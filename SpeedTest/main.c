#include "socket_ip.h"
#include "socket_http.h"
#include "speedtest.h"
#include "speedtest_utilities.h"
#include "app_debug.h"
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>



#define SPEEDTEST_DOMAIN_NAME "www.speedtest.net"
#define CONFIG_REQUEST_URL "speedtest-config.php"

#define SPEEDTEST_SERVERS_DOMAIN_NAME "c.speedtest.net"
#define SERVERS_LOCATION_REQUEST_URL "speedtest-servers-static.php?"

#define NEAREST_SERVERS_NUM 5
static struct timeval tv;
static pthread_mutex_t app_debug_mutex = PTHREAD_MUTEX_INITIALIZER; 

static uint32_t app_get_ms(void)
{
    gettimeofday(&tv, NULL);
    unsigned long long millisecondsSinceEpoch =
        (unsigned long long)(tv.tv_sec) * 1000 +
        (unsigned long long)(tv.tv_usec) / 1000;
    return millisecondsSinceEpoch;
}

static unsigned int app_puts(const void *buffer, uint32_t len)
{
    fwrite(buffer, 1, len, stdout);
    return len;
}
static bool app_lock(bool lock, uint32_t timeout)
{
    (void)timeout; /*Lock forever this case*/
    if(lock)
    {
        return pthread_mutex_lock(&app_debug_mutex);
    }
    else
    {
        return pthread_mutex_unlock(&app_debug_mutex);
    }
}

int main(int argc, char *argv[])
{
    int i, best_server_index;
    client_data_t client_data;
    server_data_t nearest_servers[NEAREST_SERVERS_NUM];
    pthread_t pid;
    struct addrinfo servinfo;
    struct itimerval timerVal;
    app_debug_register_callback_print(app_puts); /*Install callback print function*/
    app_debug_init(app_get_ms, app_lock);
    memset(&client_data, 0, sizeof(client_data_t));
    for (i = 0; i < NEAREST_SERVERS_NUM; i++) 
    {
        memset(&nearest_servers[i], 0, sizeof(server_data_t));
    }
    if (socket_ipv4_get_from_url(SPEEDTEST_DOMAIN_NAME, "http", &servinfo)) 
    {
        if (!socket_http_get_file((struct sockaddr_in *)servinfo.ai_addr, SPEEDTEST_DOMAIN_NAME, CONFIG_REQUEST_URL, CONFIG_REQUEST_URL)) 
        {
            DEBUG_ERROR("Can't get your IP address information.\n");
            return 0;
        }
    }
    if(socket_ipv4_get_from_url(SPEEDTEST_SERVERS_DOMAIN_NAME, "http", &servinfo)) 
    {
        if(!socket_http_get_file((struct sockaddr_in *)servinfo.ai_addr, SPEEDTEST_SERVERS_DOMAIN_NAME, SERVERS_LOCATION_REQUEST_URL, SERVERS_LOCATION_REQUEST_URL)) {
            DEBUG_ERROR("Can't get servers list.\n");
            return 0;
        }
    }
    st_utilities_get_ip_address_position(CONFIG_REQUEST_URL, &client_data);
    DEBUG_INFO("============================================\n");
    DEBUG_INFO("Your IP Address : %s\n", client_data.ipAddr);
    printf("Your IP Location: %f, %f\n", client_data.latitude, client_data.longitude);
    DEBUG_INFO("Your ISP        : %s\n", client_data.isp);
    DEBUG_INFO("============================================\n");
    if(st_utilities_get_nearest_server(client_data.latitude, client_data.longitude, nearest_servers) == 0) 
    {
        DEBUG_INFO("Can't get server list.\n"); 
        return 0;
    }
    if((best_server_index = st_utilities_get_best_server(nearest_servers)) != -1) 
    {
        DEBUG_INFO("==========The best server information==========\n");
        DEBUG_INFO("URL: %s\n", nearest_servers[best_server_index].url);
        printf("Latitude: %lf, Longitude: %lf\n", nearest_servers[best_server_index].latitude, nearest_servers[best_server_index].longitude);
        DEBUG_INFO("Name: %s\n", nearest_servers[best_server_index].name);
        DEBUG_INFO("Country: %s\n", nearest_servers[best_server_index].country);
        printf("Distance: %lf (km)\n", nearest_servers[best_server_index].distance);
        printf("Latency: %d (us)\n", nearest_servers[best_server_index].latency);
        DEBUG_INFO("===============================================\n");
        speedtest_download(&nearest_servers[best_server_index], 3);

    }
    return 0;
}