/**
 * @file socket_http.h
 * @brief Header file containing functions and macros related to HTTP socket operations.
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
#include <stdbool.h>
#include "app_debug.h"
#include <errno.h>
#include "socket_http.h"

/**
 * @brief Macro for verbose debugging messages.
 */
#if (DEBUG_LEVEL_VERBOSE >= DEBUG_SOCKET_HTTP_LEVEL)
#define DEBUG_SOCKET_HTTP_VERBOSE(format_, ...)  DEBUG_VERBOSE(format_, ##__VA_ARGS__)
#else
#define DEBUG_SOCKET_HTTP_VERBOSE(format_, ...) (void)(0)
#endif
/**
 * @brief Macro for info-level debugging messages.
 */
#if (DEBUG_LEVEL_INFO >= DEBUG_SOCKET_HTTP_LEVEL)
#define DEBUG_SOCKET_HTTP_INFO(format_, ...)  DEBUG_INFO(format_, ##__VA_ARGS__)
#else
#define DEBUG_SOCKET_HTTP_INFO(format_, ...) (void)(0)
#endif
/**
 * @brief Macro for error-level debugging messages.
 */
#if (DEBUG_LEVEL_ERROR >= DEBUG_SOCKET_HTTP_LEVEL)
#define DEBUG_SOCKET_HTTP_ERROR(format_, ...)  DEBUG_ERROR(format_, ##__VA_ARGS__)
#else
#define DEBUG_SOCKET_HTTP_ERROR(format_, ...) (void)(0)
#endif
/**
 * @brief Macro for raw debugging messages.
 */
#if (DEBUG_LEVEL_RAW >= DEBUG_SOCKET_HTTP_LEVEL)
#define DEBUG_SOCKET_HTTP_RAW(format_, ...)  DEBUG_RAW(format_, ##__VA_ARGS__)
#else
#define DEBUG_SOCKET_HTTP_RAW(format_, ...) (void)(0)
#endif

#define FILE_DIRECTORY_PATH "/tmp/"


/**
 * @brief Performs an HTTP GET request and saves the response to a file.
 *
 * This function establishes a connection to the specified server and sends an HTTP GET request for
 * the provided `request_url`. It saves the received response to the file specified by `filename`.
 * The function uses non-blocking IO to read data from the server and writes it to the file.
 *
 * @param serv Pointer to a sockaddr_in structure representing the server address.
 * @param domain_name The domain name of the server.
 * @param request_url The URL to request from the server.
 * @param filename The name of the file to save the HTTP response.
 * @return 1 if the HTTP request is successful and the response is saved, 0 otherwise.
 */
int socket_http_get_file(struct sockaddr_in *serv, char *domain_name, char *request_url, char *filename) 
{
    int fd;
    char sbuf[256] = {0}, tmp_path[128] = {0};
    char rbuf[8192];
    struct timeval tv;
    fd_set fdSet;
    FILE *fp = NULL;

    memset(sbuf, 0, sizeof(sbuf));
    memset(tmp_path, 0, sizeof(tmp_path));
    memset(rbuf, 0, sizeof(sbuf));
    DEBUG_SOCKET_HTTP_VERBOSE("%s: Sin family:%d\r\n", __FUNCTION__, serv->sin_family);
    if ((fd = socket(serv->sin_family, SOCK_STREAM, 0)) == -1) 
    {
        DEBUG_SOCKET_HTTP_ERROR("Open socket error!\n");
        if (fd)
        {
            close(fd);
        }
        return 0;
    }
    if (connect(fd, (struct sockaddr *)serv, sizeof(struct sockaddr)) == -1) 
    {
        DEBUG_SOCKET_HTTP_ERROR("%s: Socket connect error!\r\nDomain: %s - errno: %s\r\n", __FUNCTION__, domain_name, strerror(errno));
        if (fd) 
        {
            close(fd);
        }
        return 0;
    }
    sprintf(sbuf,
            "GET /%s HTTP/1.0\r\n"
            "Host: %s\r\n"
            "User-Agent: status\r\n"
            "Accept: */*\r\n\r\n", request_url, domain_name);                                                                 

    if(send(fd, sbuf, strlen(sbuf), 0) != strlen(sbuf)) 
    {
        DEBUG_SOCKET_HTTP_ERROR("Can't send data to server\n");
        if(fd) close(fd);
        return 0;
    }
    DEBUG_SOCKET_HTTP_INFO("%s: HTTP Get:\r\n****************************************\r\n%s\r\n****************************************\r\n", __FUNCTION__, sbuf);
    sprintf(tmp_path, "%s%s", FILE_DIRECTORY_PATH, filename);
    DEBUG_SOCKET_HTTP_INFO("%s: HTTP get directory: %s\r\n", __FUNCTION__, tmp_path);
    fp = fopen(tmp_path, "r");
    /*Perform non blocking IO read*/
    while(1) 
    {
        char *ptr=NULL;
        memset(rbuf, 0, sizeof(rbuf));
        FD_ZERO(&fdSet);
        FD_SET(fd, &fdSet);

        tv.tv_sec = 3;
        tv.tv_usec = 0;
        int status = select(fd + 1, &fdSet, NULL, NULL, &tv);
        int i = recv(fd, rbuf, sizeof(rbuf), 0);
        if (status > 0 && FD_ISSET(fd, &fdSet)) 
        {
            if (i < 0) 
            {
                printf("Can't get http file!\n");
                close(fd);
                fclose(fp);
                return 0;
            } 
            else if (i == 0) 
            {
                break;
            } 
            else 
            {
                if ((ptr = strstr(rbuf, "\r\n\r\n")) != NULL) 
                {
                    ptr += 4;
                    fwrite(ptr, 1, strlen(ptr), fp);
                } 
                else 
                {
                    fwrite(rbuf, 1, i, fp);
                }
            }
        }
    }
    close(fd);
    fclose(fp);
    return 1;
}