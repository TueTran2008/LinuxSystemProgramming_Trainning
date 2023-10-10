#ifndef __SPEED_TEST_H__
#define __SPEED_TEST_H__

#include <sys/socket.h>
#include <arpa/inet.h>
#include "speedtest.h"

/**
 * @brief Gets system uptime.
 *
 * @return Uptime in seconds, -1 on failure.
 */
float st_utilities_get_uptime(void);
/**
 * @brief Gets the IP address and position information from a file.
 *
 * @param fileName Name of the file to read.
 * @param client_data Pointer to client data structure to store IP address and position information.
 * @return 1 on success, 0 on failure.
 */
int st_utilities_get_ip_address_position(char *fileName, client_data_t *client_data);
/**
 * @brief Calculates distance between two geographical coordinates using Haversine formula.
 *
 * @param lat1 Latitude of the first point.
 * @param lon1 Longitude of the first point.
 * @param lat2 Latitude of the second point.
 * @param lon2 Longitude of the second point.
 * @return Distance in kilometers.
 */
double st_utilities_calc_distance(double lat1, double lon1, double lat2, double lon2);
/**
 * @brief Gets the nearest servers based on client's geographical coordinates.
 *
 * @param lat_c Latitude of the client.
 * @param lon_c Longitude of the client.
 * @param nearest_servers Array to store nearest server data.
 * @return 1 on success, 0 on failure.
 */
int st_utilities_get_nearest_server(double lat_c, double lon_c, server_data_t *nearest_servers);
/**
 * @brief Gets the best server based on latency.
 *
 * @param nearest_servers Array of nearest server data.
 * @param protocol Procotol support by the server
 * @return Index of the best server.
 */
int st_utilities_get_best_server(server_data_t *nearest_servers, st_server_protocol_t protocol) ;
/**
 * @brief Retrieves server information based on the provided domain name and protocol.
 *
 * This function queries server information (URL, server and domain name) using the specified domain name and protocol. The retrieved
 * server data is stored in the provided `p_target_server` structure.
 *
 * @param p_server_name The domain name of the server to query.
 * @param protocol The protocol used to communicate with the server (e.g., HTTP, HTTPS, etc.).
 * @param p_target_server Pointer to the structure where the retrieved server information will be stored.
 * @return 0 if the server information is successfully retrieved, -1 otherwise.
 */
int st_utilities_get_server_through_domain_name(char *p_server_name, st_server_protocol_t protocol, server_data_t *p_target_server);
#endif