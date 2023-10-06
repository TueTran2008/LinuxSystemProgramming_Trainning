#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include "read_line.h"  /* Declaration of readLine() */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT_NUM "50000"  /* Port number for server */
#define INT_LEN 30        /* Size of string able to hold largest integer (including terminating '\n') */

/**
 * @brief Establishes a connection to the server.
 *
 * This function initializes and establishes a connection to the server specified by
 * the given server name.
 *
 * @param p_input_server_name Server name or IP address to connect to.
 */
void socket_client(char *p_input_server_name);

#endif /* SOCKET_CLIENT_H */
