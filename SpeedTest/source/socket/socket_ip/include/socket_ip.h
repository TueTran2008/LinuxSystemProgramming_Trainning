/**
 * @file socket_ip.h
 * @brief Header file containing functions and macros related to IPv4 socket operations.
 */

#ifndef SOCKET_IP_H
#define SOCKET_IP_H

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "app_debug.h"

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
int socket_ipv4_get_from_url(char *p_input_server_name, char *p_port, struct addrinfo *p_servinfo);

/**
 * @brief Debug level for socket IP operations.
 *
 * This macro defines the debug level for socket IP operations. Modify this value to control
 * the verbosity of debug messages related to socket IP operations.
 */
#define DEBUG_SOCKET_IP_LEVEL DEBUG_LEVEL_ERROR

#endif /* SOCKET_IP_H */
