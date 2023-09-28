#ifndef __FILE_HANDLE_H__
#define __FILE_HANDLE_H__
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Read the file data to a string
 * 
 * @param[in] dir Pointer to file directory string
 * @param[in] text Pointer to readed data
 * @return 0 if success
 *         1 if fail
 */
int file_handle_read(char *dir, char *text);
/**
 * @brief Write the file data to a string
 * 
 * @param[in] dir Pointer to file directory string
 * @param[in] text Pointer to written data
 * @return 0 if success
 *         1 if fail
 */
void file_handle_write(char *dir, char *text);
/**
 * @brief Check if directory is empty
 * 
 * @param[in] file_name pointer to file directory
 * @return TRUE if directory is empty
 *         FALSE if directory isnot empty
 */
bool file_handle_check_empty_dir(const char file_name[]);
/**
 * @brief Check if file is exist
 * 
 * @param[in] file_name pointer to file directory
 * @return TRUE if directory is exist
 *         FALSE if directory isnot exist
 *
 * */
int file_handle_check_file_exist(const char file_name[]);
/**
 * @brief Read current directory of program
 * @param[in] p_out Pointer to buffer hold directory data. This buffer should be large enough 
 * @return 1 if fail
 *         0 if success
 */
int file_handle_get_current_directory(char *p_out);
/**
 * @brief Check if directory existsMAX_FILE_SIZE
 * 
 * @param[out] path Pointer to directory string
 * @return TRUE if directory exist
 *         FALSE if directory doesn't exist
 */
bool file_handle_directory_exists(const char *path);
/**
 * @brief Check if file name qualified
 * 
 * @param[in] file_name Pointer to input file name checked
 * @return TRUE if the file name is valid
 *         FALSE if the file name is invalid
 */
bool file_handle_is_valid_file_name(const char *file_name);
/**
 * @brief Create new file with the new file name
 * 
 * @param[in] new_file_name Pointer to a character buffer where the new file name will be stored.
 * @param[in] file_name The name of the input file.
 * @param[out] output_name The desired name for the output file.
 * @param[out] output_dir The directory path where the output file will be create
 */
void file_handle_create_new_file_name(char *new_file_name, char *file_name, char *output_name, char *output_dir);
/**
 * @brief Read the file data to a string
 * 
 * @param[in] file_name Pointer to directory of the file
 * @return file size in interger
 */
long file_handle_get_file_size(char file_name[]);
#endif