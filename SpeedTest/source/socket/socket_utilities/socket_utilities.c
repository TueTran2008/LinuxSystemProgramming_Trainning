
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

#include <sys/socket.h>
#include <fcntl.h>
#include <poll.h>
#include <time.h>

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

int socket_utilities_connect_timeout(int socket_fd, struct sockaddr *server, socklen_t  sock_len, unsigned int timeout) 
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
    printf("Error fcntl(..., F_GETFL) (%s)\n", strerror(errno)); 
    return -1;
  } 
  arg |= O_NONBLOCK; 
  if (fcntl(socket_fd, F_SETFL, arg) < 0) 
  { 
    printf("Error fcntl(..., F_SETFL) (%s)\n", strerror(errno)); 
    return -1;
  } 
  res = connect(socket_fd, server, sock_len); 
  if (res < 0) 
  { 
    if (errno == EINPROGRESS) 
    { 
        printf("EINPROGRESS in connect() - selecting\n"); 
        do 
        { 
            printf("In connection\r\n");
           tv.tv_sec = 1; 
           tv.tv_usec = 0; 
           FD_ZERO(&myset); 
           FD_SET(socket_fd, &myset); 
           res = select(socket_fd + 1, NULL, &myset, NULL, &tv); 
           if (res < 0 && errno != EINTR) 
           { 
              printf("Error connecting %d - %s\n", errno, strerror(errno)); 
              return -1;
           } 
           else if (res > 0) 
           { 
              // Socket selected for write 
              lon = sizeof(int); 
              if (getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0) 
              { 
                 printf("Error in getsockopt() %d - %s\n", errno, strerror(errno)); 
                 return -1;
              } 
              // Check the value returned... 
              if (valopt) 
              { 
                 printf("Error in delayed connection() %d - %s\n", valopt, strerror(valopt)); 
                 return -1;
              } 
              break; 
           } 
           else 
           { 
              printf("Timeout in select() - Cancelling!\n"); 
              return -1;
           } 
        } 
        while (1); 
     } 
     else 
     { 
        printf("Error connecting %d - %s\n", errno, strerror(errno)); 
        return -1;
     } 
     return 0;
  } 
  // Set to blocking mode again... 
  if ((arg = fcntl(socket_fd, F_GETFL, NULL)) < 0) 
  { 
      printf("Error fcntl(..., F_GETFL) (%s)\n", strerror(errno)); 
      exit(0); 
  }
  else
  {
      printf("Set socket to blocking mode again\r\n");
  } 
  arg &= (~O_NONBLOCK); 
  if (fcntl(socket_fd, F_SETFL, arg) < 0) 
  { 
      printf("Error fcntl(..., F_SETFL) (%s)\n", strerror(errno)); 
      exit(0); 
  }
  else
  {
      printf("Set socket to blocking mode again\r\n");
  } 
}

// int socket_utilities_connect_timeout(int socket_fd, const struct sockaddr *server, socklen_t sock_len, unsigned int timeout)
// {
//     int rc = 0;
//     // Set O_NONBLOCK
//     int socket_fd_flags_before;
//     if((socket_fd_flags_before=fcntl(socket_fd,F_GETFL,0)<0)) return -1;
//     if(fcntl(socket_fd,F_SETFL,socket_fd_flags_before | O_NONBLOCK)<0) return -1;
//     // Start connecting (asynchronously)
//     do {
//         if (connect(socket_fd, server, sock_len)<0) {
//             // Did connect return an error? If so, we'll fail.
//             if ((errno != EWOULDBLOCK) && (errno != EINPROGRESS)) {
//                 rc = -1;
//             }
//             // Otherwise, we'll wait for it to complete.
//             else {
//                 // Set a deadline timestamp 'timeout' ms from now (needed b/c poll can be interrupted)
//                 struct timespec now;
//                 if(clock_gettime(CLOCK_MONOTONIC, &now)<0) 
//                 { 
//                   rc=-1; 
//                   break; 
//                }
//                 struct timespec deadline = { .tv_sec = now.tv_sec,
//                                              .tv_nsec = now.tv_nsec + timeout*1000000l};
//                 // Wait for the connection to complete.
//                 do {
//                     // Calculate how long until the deadline
//                     if(clock_gettime(CLOCK_MONOTONIC, &now)<0) { rc=-1; break; }
//                     int ms_until_deadline = (int)(  (deadline.tv_sec  - now.tv_sec)*1000l
//                                                   + (deadline.tv_nsec - now.tv_nsec)/1000000l);
//                     if(ms_until_deadline<0) { rc=0; break; }
//                     // Wait for connect to complete (or for the timeout deadline)
//                     struct pollfd pfds[] = { { .fd = socket_fd, .events = POLLOUT } };
//                     rc = poll(pfds, 1, ms_until_deadline);
//                     // If poll 'succeeded', make sure it *really* succeeded
//                     if(rc>0) {
//                         int error = 0; socklen_t len = sizeof(error);
//                         int retval = getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &error, &len);
//                         if(retval==0) errno = error;
//                         if(error!=0) rc=-1;
//                     }
//                 }
//                 // If poll was interrupted, try again.
//                 while(rc==-1 && errno==EINTR);
//                 // Did poll timeout? If so, fail.
//                 if(rc==0) {
//                     errno = ETIMEDOUT;
//                     rc=-1;
//                 }
//             }
//         }
//     } while(0);
//     // Restore original O_NONBLOCK state
   
//     if(fcntl(socket_fd,F_SETFL,socket_fd_flags_before)<0) 
//     {
//        DEBUG_SOCKET_UTILITIES_VERBOSE("Restore non block state\r\n");
//       return -1;
//     }

//     return rc;
// }