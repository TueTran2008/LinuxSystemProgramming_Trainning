
#include <string.h>
#include "utilities.h"
#include <time.h>
#include <stdint.h>


#define TIME_BUF_SIZE 1000

void utilities_to_upper_case(char *buffer)
{
    uint16_t i = 0;
    while (buffer[i] && i < 256)
    {
        if (buffer[i] >= 97 && buffer[i] <= 122)
            buffer[i] = buffer[i] - 32;
        i++;
    }
}

uint16_t utilities_calculate_checksum(uint8_t *buffer, uint16_t start_addr, uint16_t length)
{
    uint32_t tmp_checksum = 0;
    uint16_t i = 0;

    for (i = start_addr; i < start_addr + length; i++)
        tmp_checksum += buffer[i];

    return (uint16_t)(tmp_checksum);
}

/***************************************************************************************************************************/
/*
 * 	Tinh check sum CRC 16 
 *
 */
#define ISO15693_PRELOADCRC16 0xFFFF
#define ISO15693_POLYCRC16 0x8408
#define ISO15693_MASKCRC16 0x0001
#define ISO15693_RESIDUECRC16 0xF0B8

uint16_t utilities_calculate_crc16(uint8_t *data_in, uint32_t size)
{
    int16_t i, j;
    int32_t res_crc = ISO15693_PRELOADCRC16;

    for (i = 0; i < size; i++)
    {
        res_crc = res_crc ^ data_in[i];
        for (j = 8; j > 0; j--)
        {
            res_crc = (res_crc & ISO15693_MASKCRC16) ? (res_crc >> 1) ^ ISO15693_POLYCRC16 : (res_crc >> 1);
        }
    }

    return ((~res_crc) & 0xFFFF);
}
uint32_t utilities_calculate_crc32(uint8_t *data, uint32_t length)
{ 
    //TODO: Forgot to implement
    return 1;
}
int ultilities_is_number(char *str) 
{
    // Iterate through each character of the string
    for (int i = 0; str[i] != '\0'; i++) {
        // Check if the character is not between '0' and '9' in ASCII
        if (str[i] < '0' || str[i] > '9') {
            return 0; // If not, return 0 (false)
        }
    }
    return 1; // If all characters are between '0' and '9', return 1 (true)
}
/* Return a string containing the current time formatted according to
   the specification in 'format' (see strftime(3) for specifiers).
   If 'format' is NULL, we use "%c" as a specifier (which gives the'
   date and time as for ctime(3), but without the trailing newline).
   Returns NULL on error. */

char *utilities_get_time(const char *format)
{
    static char buf[TIME_BUF_SIZE];  /* Nonreentrant */
    time_t t;
    size_t s;
    struct tm *tm;

    t = time(NULL);
    tm = localtime(&t);
    if (tm == NULL)
        return NULL;

    s = strftime(buf, TIME_BUF_SIZE, (format != NULL) ? format : "%c", tm);

    return (s == 0) ? NULL : buf;
}


int16_t utilities_find_index_of_char(char char_to_find, char *buffer_to_find)
{
    uint8_t tmp_count = 0;
    uint16_t length = 0;

    /* Do dai du lieu */
    length = strlen(buffer_to_find);

    for(tmp_count = 0; tmp_count < length; tmp_count++)
    {
        if(buffer_to_find[tmp_count] == char_to_find) 
            return tmp_count;
    }
    return -1;
}
uint8_t utilities_copy_parameter(char* buffer_source, char* buffer_des, char find_char_begin, char find_char_end)
{
    int16_t start_pos = utilities_find_index_of_char(find_char_begin, buffer_source);
    int16_t end_pos = utilities_find_index_of_char(find_char_end, buffer_source);
    int16_t tmp_count, i = 0;
    uint16_t number_of_copy = 0;

    /* Kiem tra dinh dang du lieu */
    if(start_pos == -1 || end_pos == -1) 
        return 0;
    if(end_pos <= 1 + start_pos) 
        return 0;
	if(end_pos >= 199 + start_pos) 
        return 0;

    for(tmp_count = start_pos + 1; tmp_count < end_pos; tmp_count++)
    {
        buffer_des[i++] = buffer_source[tmp_count];
    }
    buffer_des[i] = 0;
    return 1;
}
void utilities_reverse_string(char *str)
{
    int start = 0;
    int end = strlen(str) - 1;
    char temp;
    
    while (start < end) {
        // Swap characters at start and end indices
        temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        
        // Move the start index forward and end index backward
        start++;
        end--;
    }
}