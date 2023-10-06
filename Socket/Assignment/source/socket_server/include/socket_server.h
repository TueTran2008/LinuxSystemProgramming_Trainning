#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include "read_line.h"          /* Declaration of readLine() */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "utilities.h"

#define PORT_NUM "50000"        /* Port number for server */
#define INT_LEN 30              /* Size of string able to hold largest integer (including terminating '\n') */

/**
 * @struct socket_server
 * @brief Structure to hold data for the socket server and client information.
 */
typedef struct socket_server {
    pthread_t thread_id;       /**< Thread ID for the server thread */
    int socket_fd;             /**< Socket file descriptor for the server */
    struct sockaddr_storage cl_addr; /**< Client address information */
    socklen_t sock_len;        /**< Length of client address structure */
    char host[1024];           /**< Hostname of the client */
    char service[32];          /**< Service name of the client */
} ss_thread_data_t;

/**
 * @brief Starts the socket server.
 *
 * This function initializes and starts the socket server for the given hostname.
 *
 * @param p_input_hostname Hostname or IP address to bind the server to.
 */
void socket_server(char *p_input_hostname);

#endif /* SOCKET_SERVER_H */
