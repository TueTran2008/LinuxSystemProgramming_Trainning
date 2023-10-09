/**
 * @file speedtest_definitions.h
 * @brief Header file containing data structures and function declarations for speed test operations.
 */

#ifndef SPEEDTEST_DEFINITIONS_H
#define SPEEDTEST_DEFINITIONS_H
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
/**
 * @brief Data structure representing thread information for speed test operations.
 */
typedef struct {
    int thread_index;              /**< Index of the thread. */
    int running;                   /**< Flag indicating if the thread is running. */
    pthread_t tid;                 /**< Thread ID. */
    char domain_name[128];         /**< Domain name of the server. */
    char request_url[128];         /**< Request URL for the server. */
    struct addrinfo servinfo;   /**< Server information structure. */
} st_thread_data_t;

/**
 * @brief Data structure representing client information for speed test operations.
 */
typedef struct {
    char ipAddr[128];              /**< IP address of the client. */
    double latitude;               /**< Latitude of the client's location. */
    double longitude;              /**< Longitude of the client's location. */
    char isp[128];                 /**< Internet Service Provider of the client. */
} client_data_t;

/**
 * @brief Data structure representing server information for speed test operations.
 */
typedef struct {
    char url[128];                 /**< URL of the server. */
    double latitude;               /**< Latitude of the server's location. */
    double longitude;              /**< Longitude of the server's location. */
    char name[128];                /**< Name of the server. */
    char country[128];             /**< Country where the server is located. */
    double distance;               /**< Distance from the client in kilometers. */
    int latency;                   /**< Latency information for the server. */
    char domain_name[128];         /**< Domain name of the server. */
    struct addrinfo servinfo;   /**< Server information structure. */
} server_data_t;

/**
 * @brief Initiates speed test upload using multiple threads.
 *
 * @param nearest_server Pointer to the nearest server data.
 * @param number_of_thread Number of threads to use for upload.
 * @return 1 on success, -1 on failure.
 */
int speedtest_upload(server_data_t *nearest_server, unsigned int number_of_thread);

/**
 * @brief Initiates speed test download using multiple threads.
 *
 * @param nearest_server Pointer to the nearest server data.
 * @param number_of_thread Number of threads to use for download.
 * @return 1 on success, -1 on failure.
 */
int speedtest_download(server_data_t *nearest_server, unsigned int number_of_thread);

#endif /* SPEEDTEST_DEFINITIONS_H */
