/**
 * @file speedtest_utilities.c
 * @brief Implementation of utility functions for speed test.
 */
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include "speedtest_utilities.h"
#include "speedtest.h"
#include "socket_ip.h"
#include "socket_http.h"
#include "utilities.h"
#include <errno.h>
#include <fcntl.h>

/**
 * @brief Path to the directory where files are stored.
 */
#define FILE_DIRECTORY_PATH "/tmp/"


/**
 * @brief Request URL for servers' location data.
 */
#define SERVERS_LOCATION_REQUEST_URL "speedtest-servers-static.php?"

/**
 * @brief Gets system uptime.
 *
 * @return Uptime in seconds, -1 on failure.
 */
float st_utilities_get_uptime(void) 
{
    FILE* fp;
    float uptime, idle_time;

    if ((fp = fopen("/proc/uptime", "r"))!=NULL) 
    {
        fscanf (fp, "%f %f\n", &uptime, &idle_time);
        fclose (fp);
        return uptime;
    }
    return -1;
}
/**
 * @brief Gets the best server based on latency.
 *
 * @param nearest_servers Array of nearest server data.
 * @param protocol Procotol support by the server
 * @return Index of the best server.
 */
int st_utilities_get_best_server(server_data_t *nearest_servers, st_server_protocol_t protocol) 
{
    FILE *fp = NULL;
    int i = 0, latency, best_index = -1;
    char latency_name[16] = "latency.txt";
    char latency_file_string[16] = "test=test";
    char latency_url[NEAREST_SERVERS_NUM][128], latency_request_url[128];
    char url[128], buf[128], filePath[64] = {0}, line[64] = {0};
    struct timeval tv1, tv2;
    struct addrinfo servinfo;

    sprintf(filePath, "%s%s", FILE_DIRECTORY_PATH, latency_name);

    for (i = 0; i < NEAREST_SERVERS_NUM; i++) 
    {
        char *ptr = NULL;
        memset(latency_url[i], 0, sizeof(latency_url[i]));
        strncpy(url, nearest_servers[i].url, sizeof(nearest_servers[i].url));
        ptr = strtok(url, "/");
        while (ptr != NULL) 
        {
            memset(buf, 0, sizeof(buf));    
            strncpy(buf, ptr, strlen(ptr));

            if(strstr(buf, "upload.") != NULL) 
            {
                strcat(latency_url[i], latency_name);
            } 
            else 
            {
                strcat(latency_url[i], buf);
                strcat(latency_url[i], "/");
            }
            if (protocol == SPEEDTEST_SERVER_PROCOTOL_HTTP)
            {
                if(strstr(buf, "http:")) 
                    strcat(latency_url[i], "/");
            }
            else if (protocol == SPEEDTEST_SERVER_PROTOCOL_HTTPS)
            {
                if(strstr(buf, "https:")) 
                    strcat(latency_url[i], "/");
            }

            ptr = strtok(NULL, "/");
        }
        if (protocol == SPEEDTEST_SERVER_PROCOTOL_HTTP)
        {
            sscanf(latency_url[i], "http://%[^/]", nearest_servers[i].domain_name);
        }
        else if (protocol == SPEEDTEST_SERVER_PROTOCOL_HTTPS)
        {
            sscanf(latency_url[i], "https://%[^/]", nearest_servers[i].domain_name);
        }

        //Get request url
        memset(latency_request_url, 0, sizeof(latency_request_url));
        if((ptr = strstr(latency_url[i], nearest_servers[i].domain_name)) != NULL) {
            ptr += strlen(nearest_servers[i].domain_name);
            strncpy(latency_request_url, ptr, strlen(ptr));
        }
        if(socket_ipv4_get_from_url(nearest_servers[i].domain_name, "http", &servinfo)) 
        {
            memcpy(&nearest_servers[i].servinfo, &servinfo, sizeof(servinfo));
            gettimeofday(&tv1, NULL);
            socket_http_get_file((struct sockaddr_in *)servinfo.ai_addr, nearest_servers[i].domain_name, latency_request_url, latency_name);
            gettimeofday(&tv2, NULL);
        }

        if ((fp = fopen(filePath, "r")) != NULL) 
        {
            fgets(line, sizeof(line), fp);
            if(!strncmp(line, latency_file_string, strlen(latency_file_string)))
                nearest_servers[i].latency = (tv2.tv_sec - tv1.tv_sec) * 1000000 + tv2.tv_usec - tv1.tv_usec;
            else
                nearest_servers[i].latency = -1;

            fclose(fp);
            unlink(filePath);
        } else {
            nearest_servers[i].latency = -1;
        }
    }

    //Select low latency server
    for (i = 0; i < NEAREST_SERVERS_NUM; i++) 
    {
        if(i == 0) {
            best_index = i;
            latency = nearest_servers[i].latency;
        } else {
            if(nearest_servers[i].latency < latency && nearest_servers[i].latency != -1) {
                best_index = i;
                latency = nearest_servers[i].latency;
            }           
        }
        nearest_servers[i].servinfo = servinfo;
    }
    return best_index;
}
/**
 * @brief Gets the IP address and position information from a file.
 *
 * @param fileName Name of the file to read.
 * @param client_data Pointer to client data structure to store IP address and position information.
 * @return 1 on success, 0 on failure.
 */
int st_utilities_get_ip_address_position(char *fileName, client_data_t *client_data) 
{
    FILE *fp=NULL;
    char filePath[128] = {0}, line[512] = {0}, lat[64] = {0}, lon[64] = {0};
    sprintf(filePath, "%s%s", FILE_DIRECTORY_PATH, fileName);

    if ((fp=fopen(filePath, "r"))!=NULL) 
    {
        while(fgets(line, sizeof(line) - 1, fp)!=NULL) 
        {
            if(!strncmp(line, "<client", 7)) {
                //ex: <client ip="61.216.30.97" lat="24.7737" lon="120.9436" isp="HiNet" isprating="3.2" rating="0" ispdlavg="50329" ispulavg="22483" loggedin="0"/>        
                sscanf(line, "%*[^\"]\"%255[^\"]\"%*[^\"]\"%255[^\"]\"%*[^\"]\"%255[^\"]\"%*[^\"]\"%255[^\"]\"", client_data->ipAddr, lat, lon, client_data->isp);
                client_data->latitude = atof(lat);
                client_data->longitude = atof(lon);
                break;
            }
        }
        fclose(fp);
    }
    return 1;
}
/**
 * @brief Calculates distance between two geographical coordinates using Haversine formula.
 *
 * @param lat1 Latitude of the first point.
 * @param lon1 Longitude of the first point.
 * @param lat2 Latitude of the second point.
 * @param lon2 Longitude of the second point.
 * @return Distance in kilometers.
 */
double st_utilities_calc_distance(double lat1, double lon1, double lat2, double lon2) 
{
    int R = 6371;  //Radius of the Earth
    double dlat, dlon, a, c, d;

    dlat = (lat2-lat1) * M_PI / 180;
    dlon = (lon2-lon1)* M_PI / 180;

    a = pow(sin(dlat/2), 2) + cos(lat1*M_PI/180)*cos(lat2*M_PI/180)*pow(sin(dlon/2), 2);
    c = 2 * atan2(sqrt(a), sqrt(1-a));
    d = R * c;
    return d;
}
/**
 * @brief Gets the nearest servers based on client's geographical coordinates.
 *
 * @param lat_c Latitude of the client.
 * @param lon_c Longitude of the client.
 * @param nearest_servers Array to store nearest server data.
 * @return 1 on success, 0 on failure.
 */
int st_utilities_get_nearest_server(double lat_c, double lon_c, server_data_t *nearest_servers) 
{
    FILE *fp = NULL;
    char filePath[128] = {0}, line[512] = {0}, url[128] = {0}, lat[64] = {0}, lon[64] = {0}, name[128] = {0}, country[128] = {0};
    double lat_s, lon_s, distance;
    int count = 0, i = 0, j = 0;

    snprintf(filePath, sizeof(filePath), "%s%s", FILE_DIRECTORY_PATH, SERVERS_LOCATION_REQUEST_URL);

    if ((fp = fopen(filePath, "r")) != NULL) {
        while (fgets(line, sizeof(line) - 1, fp) != NULL) {
            if (!strncmp(line, "<server", 7)) {
                
                sscanf(line, "%*[^\"]\"%127[^\"]\"%*[^\"]\"%63[^\"]\"%*[^\"]\"%63[^\"]\"%*[^\"]\"%127[^\"]\"%*[^\"]\"%127[^\"]\"", url, lat, lon, name, country);

                lat_s = atof(lat);
                lon_s = atof(lon);

                distance = st_utilities_calc_distance(lat_c, lon_c, lat_s, lon_s);
                
                for (i = 0; i < NEAREST_SERVERS_NUM; i++)
                {
                    if (strstr(url, ":"))
                    {
                        char tmp_url[128];
                        memset(tmp_url, 0, sizeof(tmp_url));
                        char *p_tmp_url = utilities_strremove(url, ":8080");
                    }
                    if (nearest_servers[i].url[0] == '\0') 
                    {
                        strncpy(nearest_servers[i].url, url, sizeof(nearest_servers[i].url));
                        nearest_servers[i].latitude = lat_s;
                        nearest_servers[i].longitude = lon_s;
                        strncpy(nearest_servers[i].name, name, sizeof(nearest_servers[i].name));
                        strncpy(nearest_servers[i].country, country, sizeof(nearest_servers[i].country));
                        nearest_servers[i].distance = distance;
                        break;
                    } 
                    else 
                    {
                        if (distance <= nearest_servers[i].distance) 
                        {
                            memset(&nearest_servers[NEAREST_SERVERS_NUM - 1], 0, sizeof(server_data_t));
                            for (j = NEAREST_SERVERS_NUM - 1; j > i; j--) {
                                strncpy(nearest_servers[j].url, nearest_servers[j - 1].url, sizeof(nearest_servers[j - 1].url));
                                nearest_servers[j].latitude = nearest_servers[j - 1].latitude;
                                nearest_servers[j].longitude = nearest_servers[j - 1].longitude;
                                strncpy(nearest_servers[j].name, nearest_servers[j - 1].name, sizeof(nearest_servers[j - 1].name));
                                strncpy(nearest_servers[j].country, nearest_servers[j - 1].country, sizeof(nearest_servers[j - 1].country));
                                nearest_servers[j].distance = nearest_servers[j - 1].distance;
                            }
                            strncpy(nearest_servers[i].url, url, sizeof(nearest_servers[i].url));
                            nearest_servers[i].latitude = lat_s;
                            nearest_servers[i].longitude = lon_s;
                            strncpy(nearest_servers[i].name, name, sizeof(nearest_servers[i].name));
                            strncpy(nearest_servers[i].country, country, sizeof(nearest_servers[i].country));
                            nearest_servers[i].distance = distance;
                            break;
                        }
                    }
                }
                count++;
            }
        }
        fclose(fp);
    }
    // if (remove(filePath) == 0)
    // {
    //     DEBUG_ERROR("Error when delete file :%s\r\n");
    //     return 0;
    // }
    // else
    // {
    //     DEBUG_WARN("Success when delete file :%s\r\n");
    // }
    return (count > 0) ? 1 : 0;
}
/**
 * @brief Retrieves server information based on the provided domain name and protocol.
 *
 * This function queries server information (URL, server and domain name) using the specified domain name and protocol. The retrieved
 * server data is stored in the provided `p_target_server` structure.
 *
 * @param p_server_name The domain name of the server to query.
 * @param protocol The protocol used to communicate with the server (e.g., HTTP, HTTPS, etc.).
 * @param p_target_server Pointer to the structure where the retrieved server information will be stored.
 * @return 0 if the server information is successfully retrieved, -1 otherwise.
 */
int st_utilities_get_server_through_domain_name(char *p_server_name, st_server_protocol_t protocol, server_data_t *p_target_server)
{
    struct addrinfo servinfo;
    if(p_server_name == NULL)
    {
        printf("%s: Input server is NULL", __FUNCTION__);
        return -1;
    }
    if(socket_ipv4_get_from_url(p_server_name, "http", &servinfo) == 0) 
    {
        printf("Errno when resolse ip - errno:%s\r\n", strerror(errno));
        return -1;
    }
    memcpy(&p_target_server->servinfo, &servinfo, sizeof(servinfo));
    memcpy(&p_target_server->domain_name, p_server_name, strlen(p_server_name));
    if(protocol == SPEEDTEST_SERVER_PROCOTOL_HTTP)
    {
        sprintf(p_target_server->url, "http://%s/speedtest/upload.php", p_target_server->domain_name);
    }
    else if (protocol == SPEEDTEST_SERVER_PROTOCOL_HTTPS)
    {
        sprintf(p_target_server->url, "https://%s/speedtest/upload.php", p_target_server->domain_name);
    }
    else
    {
        printf("Invalid protocol");
        return -1;
    }
    return 0;
}