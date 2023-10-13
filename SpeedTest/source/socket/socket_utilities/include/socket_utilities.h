/**
 * @file socket_utilities.h
 * @brief Header file containing functions and macros related to utilitiy socket operations.
 */

#ifndef SOCKET_ULTITLIES_H
#define SOCKET_ULTITLIES_H

#include "app_debug.h"

/**
 * @brief Debug level for socket utility operations.
 *
 * This macro defines the debug level for socket utility operations. Modify this value to control
 * the verbosity of debug messages related to socket utility operations.
 */
#define DEBUG_SOCKET_UTILITIES_LEVEL DEBUG_LEVEL_VERBOSE


/**
 * @brief Connects a socket to the specified server address with a timeout.
 *
 * This function attempts to establish a connection to the server specified in the `server` parameter.
 * It sets a timeout for the connection attempt.
 *
 * @param socket_fd The socket file descriptor.
 * @param server A pointer to the sockaddr structure representing the server address.
 * @param sock_len Socket len specify by socket addressin
 * @param timeout Timeout in seconds
 * @return 0 if the connection is successful, -1 on timeout or failure.
 */
int socket_utilities_connect_timeout(int socket_fd, struct sockaddr *server, socklen_t  sock_len, unsigned int timeout);

#endif /* SOCKET_ULTILITIES_H */
