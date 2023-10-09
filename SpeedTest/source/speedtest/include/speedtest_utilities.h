#ifndef __SPEED_TEST_H__
#define __SPEED_TEST_H__

#include <sys/socket.h>
#include <arpa/inet.h>
#include "speedtest.h"

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
 * @return Index of the best server.
 */
int st_utilities_get_best_server(server_data_t *nearest_servers);
#endif