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
#include "app_debug.h"

/**
 * @brief Date enum represetings protocol support by speedtest application
 */
typedef enum
{
    SPEEDTEST_SERVER_PROCOTOL_HTTP = 0, /**< HTTP procotol. */
    SPEEDTEST_SERVER_PROTOCOL_HTTPS,    /**< HTTPS procotol. */
    SPEEDTEST_SERVER_PROTOCOL_MAX
} st_server_protocol_t;
/**
 * @brief Date enum represetings operation support by speedtest application
 */
typedef enum
{
    SPEEDTEST_SERVER_OPERATION_UPLOAD = 1, /**< Upload data test. */
    SPEEDTEST_SERVER_OPERATION_DOWNLOAD = 2,    /**< Download data test. */
    SPEEDTEST_SERVER_OPERATION_MAX
} st_server_operation_t;
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
    st_server_operation_t protocol; /**< How speedtest obtain data. */
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
 * @brief Log debugging level of the source file
 */
#define DEBUG_SPEEDTEST_LEVEL DEBUG_LEVEL_NONE
/**
 * @brief Number of nearest servers to consider.
 */
#define NEAREST_SERVERS_NUM 10

/**
 * @brief Domain name for the main Speedtest website.
 */
#define SPEEDTEST_DOMAIN_NAME "www.speedtest.net"

/**
 * @brief Request URL for retrieving Speedtest configuration data.
 */
#define CONFIG_REQUEST_URL "speedtest-config.php"

/**
 * @brief Domain name for Speedtest servers.
 */
#define SPEEDTEST_SERVERS_DOMAIN_NAME "c.speedtest.net"

/**
 * @brief Request URL for retrieving Speedtest servers' location data.
 */
#define SERVERS_LOCATION_REQUEST_URL "speedtest-servers-static.php?"
/**
 * @brief Dureation in second for each speed test task
 */
#define SPEEDTEST_DURATION 15



/**
 * @brief Initiates speed test upload using multiple threads.
 *
 * @param nearest_server Pointer to the nearest server data.
 * @param number_of_thread Number of threads to use for upload.
 * @return 1 on success, -1 on failure.
 */
int speedtest_upload(server_data_t *test_server, unsigned int number_of_thread, st_server_protocol_t protocol);

/**
 * @brief Initiates speed test download using multiple threads.
 *
 * @param nearest_server Pointer to the nearest server data.
 * @param number_of_thread Number of threads to use for download.
 * @return 1 on success, -1 on failure.
 */
int speedtest_download(server_data_t *nearest_server, unsigned int number_of_thread, st_server_protocol_t protocol);
/**
 * @brief Performs a Speedtest on a specific domain name using the given protocol, operation, and number of threads.
 *
 * @param p_domain_name The domain name to perform the Speedtest on.
 * @param protocol The protocol to be used for the Speedtest (e.g., HTTP, HTTPS).
 * @param operation The type of Speedtest operation (e.g., upload, download).
 * @param number_of_thread Number of threads to be used for the Speedtest.
 * @return 0 if the Speedtest is successful, -1 on failure.
 */
int speedtest_test_domain_name(char *p_domain_name, st_server_protocol_t protocol, st_server_operation_t operation, int number_of_thread);

/**
 * @brief Performs a Speedtest on the server with the lowest latency using the given protocol, operation, and number of threads.
 *
 * @param protocol The protocol to be used for the Speedtest (e.g., HTTP, HTTPS).
 * @param operation The type of Speedtest operation (e.g., upload, download).
 * @param number_of_thread Number of threads to be used for the Speedtest.
 * @return 0 if the Speedtest is successful, -1 on failure.
 */
int speedtest_test_lowest_latency(st_server_protocol_t protocol, st_server_operation_t operation, int number_of_thread);


#endif /* SPEEDTEST_DEFINITIONS_H */
