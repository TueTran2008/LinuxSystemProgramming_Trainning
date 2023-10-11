#ifndef __SOCKET_HTTP_H__
#define __SOCKET_HTTP_H__

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
#include "app_debug.h"

#define DEBUG_SOCKET_HTTP_LEVEL DEBUG_LEVEL_NONE //Error level

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
int socket_http_get_file(struct sockaddr_in *serv, char *domain_name, char *request_url, char *filename);
#endif