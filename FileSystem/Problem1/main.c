#define _DEFAULT_SOURCE /* Get major() and minor() from <sys/types.h> */
#include <stdio.h>
#include "file_handle.h"
#include "file_type.h"
//#include <limits.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char list_file[256][256];
    char **p_list_file = (char**)list_file;
    char temp_dir[256*2];
    int number_of_file = 0;
    struct stat sb;
    int ret = 0;
    bool is_current_directory = false;
    if (argc < 2|| strcmp(argv[1], "--help") == 0)
    {
        printf("Listing file type name: <Input Path>\r\n");
        return 0;
    }
    if (strcmp(argv[1], ".") == 0)
    {
        is_current_directory = true;
    }
    ret = file_handle_directory_exists(argv[1]);
    if (ret == 0)    /*Not a directory*/
    {
        printf("Input is not a directory\r\n");
        return 0;
    }
    ret = file_type_list_file(argv[1], list_file, &number_of_file);
    if (ret)/*Error if not equal to zero with this function*/
    {
        printf("Error when listing file\r\n");
        return 0;
    }
    for (int i = 0; i < (int)number_of_file; i++)
    {
        if (is_current_directory)
        {
            sprintf(temp_dir, "%s/%s", argv[1], list_file[i]); /*Concate a file name directory. Path/filename*/
        }
        else
        {
            sprintf(temp_dir, "%s%s", argv[1], list_file[i]);
        }
        if (stat(temp_dir, &sb) == -1)
        {
            printf("Error when take stat index:%i", i);
        }
        printf("%s - ", list_file[i]); /*Display file name*/
        file_type_display_type(&sb);
        printf("\r\n");
    }
    return 0;
}