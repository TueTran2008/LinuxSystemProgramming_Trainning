/******************************************************************************
 *                              HEADER AND DIRECTORY 
******************************************************************************/
#include "file_handle.h"
#include <stdio.h>
#include <limits.h>
/******************************************************************************
 *                              DEFINE AND TYPEDEF 
******************************************************************************/
#if(DEBUG_FILE_ENABLE)
#define DEBUG_FILE(format_, ...)  printf(format_, ##__VA_ARGS__)
#else
#define DEBUG_FILE(format_, ...) (void)(0)
#endif

/******************************************************************************
 *                              GLOABAL FUNCTION 
******************************************************************************/
int file_handle_read(char *dir, char *text)
{
    FILE *fp = NULL;
    fp = fopen(dir, "r");
    if (fp != NULL) {
        char ch;
        int index = 0;
        while ((ch = fgetc(fp)) != EOF) {
            if (ch == '\n') {
                text[index++] = '|'; // Use '|' to indicate line breaks
                text[index++] = ' ';
            } else {
                text[index++] = ch;
            }
        }
        text[index] = '\0'; // Null-terminate the string
        fclose(fp);
        return 0;
    }
    else
    {
        return 1;
    }
}

void file_handle_write(char *dir, char *text)
{
    FILE *fp = NULL;
    fp = fopen(dir, "w");
    fprintf(fp, "%s", text);
    fclose(fp);
}

// Check empty dir
bool file_handle_check_empty_dir(const char file_name[])
{
    int index = 0;

    for (index; index < strlen(file_name); index++) {
        if (file_name[index] != '\\' && file_name[index] != '/' && file_name[index] != '.') {
            return true;
        }
    }

    return false;
}

// Check if file exist
int file_handle_check_file_exist(const char file_name[])
{
    FILE *file = NULL;

    if (file_name == NULL) {
        return 0;
    }

    if (!file_handle_check_empty_dir(file_name)) {
        return 0;
    }

    if ((file = fopen(file_name, "r"))) {
        fclose(file);
        return 1;
    }
    return 0;
}

// Get current dir
int file_handle_get_current_directory(char *p_out) 
{
    int ret = 0;
    char *buffer = (char*)malloc(sizeof(char) * 4096);

    if (getcwd(buffer, 4096) == NULL) {
        perror("Error getting current directory");
        free(buffer);
        return 1;
    }
    strcpy(p_out, buffer);
    free(buffer);
    return 0;
}

// Check if directory exist
bool file_handle_directory_exists(const char *path) 
{
    if (access(path, F_OK) != -1) {
        return true; // directory exists
    } else {
        return false; // directory does not exist
    }
}

// Check if file name is valid
bool file_handle_is_valid_file_name(const char *file_name) 
{
    const char *invalid_chars = "\\/:*?\"<>|";

    // Check if the file name is empty or exceeds the maximum length
    if (strlen(file_name) == 0 || strlen(file_name) > FILENAME_MAX) {
        return false;
    }
    
    // Check if the file name contains any invalid characters
    for (int i = 0; i < strlen(invalid_chars); i++) {
        if (strchr(file_name, invalid_chars[i]) != NULL) {
            return false;
        }
    }
    
    // Check if the file name starts or ends with a space or a period
    if (file_name[0] == ' ' || file_name[0] == '.' || 
        file_name[strlen(file_name)-1] == ' ' || file_name[strlen(file_name)-1] == '.') {
        return false;
    }
    
    return true;
}

// Create new file name with the directory and extention of old file name 
void file_handle_create_new_file_name(char *new_file_name, char *file_name, char *output_name, char *output_dir)
{
    // Get file extension
    char *ext = strrchr(file_name, '.');
    if (!ext) {
        ext = "";
    } else {
        ext = ext + 1;
    }

    // Make new file name
    strcpy(new_file_name, output_dir);
    strcat(new_file_name, "/");
    strcat(new_file_name, output_name);
    strcat(new_file_name, ".");
    strcat(new_file_name, ext);
}

long file_handle_get_file_size(char file_name[]) {
    FILE *fp = fopen(file_name, "r");

    if (fp == NULL)
        return -1;

    if (fseek(fp, 0, SEEK_END) < 0) {
        fclose(fp);
        return -1;
    }

    long size = ftell(fp);
    // release the resources when not required
    fclose(fp);
    return size;
}
