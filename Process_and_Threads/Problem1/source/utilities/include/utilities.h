#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <stdint.h>
#include <stdbool.h>

/*!
 * @brief           Calculate checksum of a buffer
 * @param[in]       buffer Data
 * @param[in]       begin_addr  Starting point of checksum data
 * @param[in]       length Buffer length
 * @retval          Checksum of buffer
 */
uint16_t utilities_calculate_checksum(uint8_t* buffer, uint16_t begin_addr, uint16_t length);

/*!
 * @brief           Calculate CRC16 of data
 * @param[in]       data Data
 * @param[in]       length Data length
 * @retval          CRC16 of buffer
 */
uint16_t utilities_calculate_crc16(uint8_t *data, uint32_t length);

/*!
 * @brief           To upper case 
 * @param[in]       str String convert to upper case
 */
void utilities_to_upper_case(char* str);

/*!
 * @brief           Calculate CRC32 of data
 * @param[in]       data Data
 * @param[in]       length Data length
 * @retval          CRC32 of buffer
 */
uint32_t utilities_calculate_crc32(uint8_t *data, uint32_t length);
/*!
 * @brief           Check the input string is a number
 * @param[in]       str Pointer to string data
 * @retval          1 if is a number
 *                  0 if not a numer
 */
int ultilities_is_number(char *str);
/*!
 * @brief           Check the input string is a number
 * @param[in]       str Pointer to string data
 * @retval          1 if is a number
 *                  0 if not a numer
 */
char *utilities_get_time(const char *format);

#endif // __UTILITIES_H__