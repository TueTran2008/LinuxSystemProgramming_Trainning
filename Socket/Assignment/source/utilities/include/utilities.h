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
 * @brief           Get the string of date and time 
 * @param[in]       format Format of the output string
 */
char *utilities_get_time(const char *format);
/*!
 * @brief           Get the string of date and time 
 * @param[in]       char_to_find Character we want to find in string
 * @param[in]       buffer_to_find Pointer point to string when want to filter
 */
int16_t utilities_find_index_of_char(char char_to_find, char *buffer_to_find);
/*!
 * @brief           Get the string of date and time 
 * @param[in]       buffer_source Pointer to source buffer want to find
 * @param[out]      buffer_des  Pointer point to buffer we want data to copy form begin to end
 * @param[in]       find_char_begin Character we want to begin from
 * @param[in]       find_char_end Character we want to end with
 */
uint8_t utilities_copy_parameter(char* buffer_source, char* buffer_des, char find_char_begin, char find_char_end);
/*!
 * @brief           Reverse the string
 * @param[in]       str Pointer to string to reverse
 */
void utilities_reverse_string(char *str);

#endif // __UTILITIES_H__