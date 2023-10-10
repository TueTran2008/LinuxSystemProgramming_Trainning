/******************************************************************************
 *                              HEADER AND DIRECTORY 
******************************************************************************/

/**
 * @file socket_ip.c
 * @brief Implementation file for socket IP operations.
 */

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include "app_debug.h"

#if (DEBUG_LEVEL_VERBOSE >= DEBUG_SOCKET_IP_LEVEL)
#define DEBUG_SOCKET_IP_VERBOSE(format_, ...)  DEBUG_VERBOSE(format_, ##__VA_ARGS__)
#else
#define DEBUG_SOCKET_IP_VERBOSE(format_, ...) (void)(0)
#endif
#if (DEBUG_LEVEL_INFO >= DEBUG_SOCKET_IP_LEVEL)
#define DEBUG_SOCKET_IP_INFO(format_, ...)  DEBUG_INFO(format_, ##__VA_ARGS__)
#else
#define DEBUG_SOCKET_IP_INFO(format_, ...) (void)(0)
#endif
#if (DEBUG_LEVEL_ERROR >= DEBUG_SOCKET_IP_LEVEL)
#define DEBUG_SOCKET_IP_ERROR(format_, ...)  DEBUG_ERROR(format_, ##__VA_ARGS__)
#else
#define DEBUG_SOCKET_IP_ERROR(format_, ...) (void)(0)
#endif
#if (DEBUG_LEVEL_RAW >= DEBUG_SOCKET_IP_LEVEL)
#define DEBUG_SOCKET_IP_RAW(format_, ...)  DEBUG_RAW(format_, ##__VA_ARGS__)
#else
#define DEBUG_SOCKET_IP_RAW(format_, ...) (void)(0)
#endif

/**
 * @brief Checks if a port string is valid.
 *
 * This function checks if the input port string is valid by searching for "http".
 *
 * @param p_input_port The input port string.
 * @return true if the port string contains "http", false otherwise.
 */
static bool port_string_is_valid(char *p_input_port)
{
    char *http = strstr(p_input_port, "http");
    if(http == NULL)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/**
 * @brief Resolves the IPv4 address of the given server name and port number.
 *
 * This function resolves the IPv4 address of the specified server name and port number
 * and populates the provided addrinfo structure with the result.
 *
 * @param p_input_server_name The server name or IP address to resolve.
 * @param p_port The port number to connect to.
 * @param p_servinfo Pointer to the addrinfo structure to store the resolved information.
 */
int socket_ipv4_get_from_url(char *p_input_server_name, char *p_port, struct addrinfo *p_servinfo)
{
    int cfd;
    ssize_t num_read;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    
    if(p_input_server_name == NULL || p_port == NULL || p_servinfo == NULL)
    {
        DEBUG_SOCKET_IP_ERROR("%s - line %d: NULL input\r\n", __FUNCTION__, __LINE__);
        return 0;
    }
    /* Call getaddrinfo() to obtain a list of addresses that
       we can try connecting to */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;                /* Allows IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    if (getaddrinfo(p_input_server_name, p_port, &hints, &result) != 0)
    {
        DEBUG_SOCKET_IP_ERROR("Resolve DNS Failed: Can't get ip address! (%s) -- errno: %d\n", p_input_server_name, errno);
        return 0;     
    }
    else
    {
        DEBUG_SOCKET_IP_INFO("Resolve DNS Successfully(%s) -- errno: %d\n", p_input_server_name, errno);
    }
    for (rp = result; rp != NULL; rp = rp->ai_next) 
    {
        if(rp->ai_family == AF_INET)
        {
            memcpy(p_servinfo, rp, sizeof(struct addrinfo));
            break;
        }
    }
    freeaddrinfo(result);
    return 1;
}
