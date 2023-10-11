#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Calculate the checksum of a buffer.
 *
 * @param buffer The input buffer.
 * @param start_addr The starting address in the buffer.
 * @param length The length of the data to calculate the checksum for.
 * @return The calculated checksum.
 */
uint16_t utilities_calculate_checksum(uint8_t* buffer, uint16_t begin_addr, uint16_t length);

/*!
 * @brief           Calculate CRC16 of data
 * @param[in]       data Data
 * @param[in]       length Data length
 * @retval          CRC16 of buffer
 */
uint16_t utilities_calculate_crc16(uint8_t *data, uint32_t length);

/**
 * @brief Convert a string to uppercase.
 *
 * @param buffer The input string to be converted to uppercase.
 */
void utilities_to_upper_case(char *buffer);

/**
 * @brief Calculate the CRC32 checksum of a buffer (Not implemented).
 *
 * @param data The input buffer.
 * @param length The length of the buffer.
 * @return The calculated CRC32 checksum.
 */
uint32_t utilities_calculate_crc32(uint8_t *data, uint32_t length);
/**
 * @brief Check if a string represents a number.
 *
 * @param str The input string.
 * @return 1 if the string represents a number, 0 otherwise.
 */
int ultilities_is_number(char *str);
/**
 * @brief Get the current time formatted according to the specified format.
 *
 * @param format The format specifier (see strftime(3) for details).
 * @return A string containing the formatted time.
 */
char *utilities_get_time(const char *format);
/**
 * @brief Find the index of a character in a string.
 *
 * @param char_to_find The character to find.
 * @param buffer_to_find The input string to search in.
 * @return The index of the character in the string, or -1 if not found.
 */
int16_t utilities_find_index_of_char(char char_to_find, char *buffer_to_find);
/**
 * @brief Copy a parameter from a source string to a destination string.
 *
 * @param buffer_source The source string containing the parameter.
 * @param buffer_des The destination string to copy the parameter to.
 * @param find_char_begin The character indicating the start of the parameter.
 * @param find_char_end The character indicating the end of the parameter.
 * @return 1 if successful, 0 if the format is invalid or the data cannot be copied.
 */
uint8_t utilities_copy_parameter(char* buffer_source, char* buffer_des, char find_char_begin, char find_char_end);
/**
 * @brief Reverse a string in place.
 *
 * @param str The input string to be reversed.
 */
void utilities_reverse_string(char *str);
/**
 * @brief Remove all occurrences of a substring from a string.
 *
 * @param str The input string.
 * @param sub The substring to be removed.
 * @return The modified string.
 */
char *utilities_strremove(char *str, const char *sub);

#endif // __UTILITIES_H__