
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
#include "socket_ip.h"
#include "socket_utilities.h"
#include <stdlib.h>
#include <fcntl.h>

/**
 * @brief Macro for verbose debugging messages.
 */
#if (DEBUG_LEVEL_VERBOSE >= DEBUG_SOCKET_UTILITIES_LEVEL)
#define DEBUG_SOCKET_UTILITIES_VERBOSE(format_, ...)  DEBUG_VERBOSE(format_, ##__VA_ARGS__)
#else
#define DEBUG_SOCKET_UTILITIES_VERBOSE(format_, ...) (void)(0)
#endif
/**
 * @brief Macro for information debugging messages.
 */
#if (DEBUG_LEVEL_INFO >= DEBUG_SOCKET_UTILITIES_LEVEL)
#define DEBUG_SOCKET_UTILITIES_INFO(format_, ...)  DEBUG_INFO(format_, ##__VA_ARGS__)
#else
#define DEBUG_SOCKET_UTILITIES_INFO(format_, ...) (void)(0)
#endif
/**
 * @brief Macro for error debugging messages.
 */
#if (DEBUG_LEVEL_ERROR >= DEBUG_SOCKET_UTILITIES_LEVEL)
#define DEBUG_SOCKET_UTILITIES_ERROR(format_, ...)  DEBUG_ERROR(format_, ##__VA_ARGS__)
#else
#define DEBUG_SOCKET_UTILITIES_ERROR(format_, ...) (void)(0)
#endif
/**
 * @brief Macro for raw debugging messages.
 */
#if (DEBUG_LEVEL_RAW >= DEBUG_SOCKET_UTILITIES_LEVEL)
#define DEBUG_SOCKET_UTILITIES_RAW(format_, ...)  DEBUG_RAW(format_, ##__VA_ARGS__)
#else
#define DEBUG_SOCKET_UTILITIES_RAW(format_, ...) (void)(0)
#endif

int socket_utilities_connect_timeout(int socket_fd, struct sockaddr *server) 
{ 
  int res; 
  struct sockaddr_in addr; 
  long arg; 
  fd_set myset; 
  struct timeval tv; 
  int valopt; 
  socklen_t lon; 
  if ((arg = fcntl(socket_fd, F_GETFL, NULL)) < 0) 
  { 
    DEBUG_SOCKET_UTILITIES_ERROR("Error fcntl(..., F_GETFL) (%s)\n", strerror(errno)); 
    return -1;
  } 
  arg |= O_NONBLOCK; 
  if( fcntl(socket_fd, F_SETFL, arg) < 0) 
  { 
    DEBUG_SOCKET_UTILITIES_ERROR("Error fcntl(..., F_SETFL) (%s)\n", strerror(errno)); 
    return -1;
  } 
  res = connect(socket_fd, server, sizeof(addr)); 
  if (res < 0) 
  { 
    if (errno == EINPROGRESS) 
    { 
        DEBUG_SOCKET_UTILITIES_VERBOSE("EINPROGRESS in connect() - selecting\n"); 
        do 
        { 
           tv.tv_sec = 15; 
           tv.tv_usec = 0; 
           FD_ZERO(&myset); 
           FD_SET(socket_fd, &myset); 
           res = select(socket_fd + 1, NULL, &myset, NULL, &tv); 
           if (res < 0 && errno != EINTR) 
           { 
              DEBUG_SOCKET_UTILITIES_ERROR("Error connecting %d - %s\n", errno, strerror(errno)); 
              return -1;
           } 
           else if (res > 0) 
           { 
              // Socket selected for write 
              lon = sizeof(int); 
              if (getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0) 
              { 
                 DEBUG_SOCKET_UTILITIES_ERROR("Error in getsockopt() %d - %s\n", errno, strerror(errno)); 
                 return -1;
              } 
              // Check the value returned... 
              if (valopt) 
              { 
                 DEBUG_SOCKET_UTILITIES_ERROR("Error in delayed connection() %d - %s\n", valopt, strerror(valopt)); 
                 return -1;
              } 
              break; 
           } 
           else 
           { 
              DEBUG_SOCKET_UTILITIES_ERROR("Timeout in select() - Cancelling!\n"); 
              return -1;
           } 
        } 
        while (1); 
     } 
     else 
     { 
        DEBUG_SOCKET_UTILITIES_ERROR("Error connecting %d - %s\n", errno, strerror(errno)); 
        return -1;
     } 
     return 0;
  } 
  // Set to blocking mode again... 
  if ((arg = fcntl(socket_fd, F_GETFL, NULL)) < 0) 
  { 
      DEBUG_SOCKET_UTILITIES_ERROR("Error fcntl(..., F_GETFL) (%s)\n", strerror(errno)); 
      exit(0); 
  } 
  arg &= (~O_NONBLOCK); 
  if (fcntl(socket_fd, F_SETFL, arg) < 0) 
  { 
      DEBUG_SOCKET_UTILITIES_ERROR("Error fcntl(..., F_SETFL) (%s)\n", strerror(errno)); 
      exit(0); 
  } 
}