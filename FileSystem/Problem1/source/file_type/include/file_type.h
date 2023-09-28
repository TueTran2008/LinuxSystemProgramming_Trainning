#ifndef __FILE_TYPE_H__
#define __FILE_TYPE_H__
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define DEBUG_FILE_TYPE_ENABLE 0
#define DISPLAY_ONLY_FILE_TYPE 1
#define FILE_MAX_IN_FOLDER 256

/**
 * @brief Read the file data to a string
 * 
 * @param[in] input_dir Pointer to input directory string
 * @param[in] p_out Pointer to two demention array hold output data
 * @param[out] number_of_file  The number of file in the directory
 * @return 0 if success
 *         1 if fail
 */
int file_type_list_file(const char* input_dir, char p_out[][256], int *number_of_file);
/**
 * @brief Display the File type Attribute (ex, Regular, Directory, Special(character, block, socket, ...))
 * 
 * @param[in] sb Pointer to stat structure, which holds information about file (Like Inode)
 */
void file_type_display_type(const struct stat *sb);
#endif