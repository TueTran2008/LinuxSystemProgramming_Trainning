#include "file_link.h"
#include "file_handle.h"
#include <unistd.h>
#include <errno.h>

int file_link_create(file_link_t file_link, char *input_old_path, char *input_new_path)
{
    int ret = 0;
    if (input_old_path == NULL || input_new_path == NULL)
    {
        printf("%s input NULL value\r\n", __FUNCTION__);
        return 1;
    }
    char name_buf[strlen(input_new_path) + 2];
    char *path_new = name_buf;
    memset(path_new, 0, sizeof(name_buf));
    ret = file_handle_check_file_exist(input_old_path);
    if (ret == 0)
    {
        printf("%s file %s doesn't exist\r\n", __FUNCTION__, input_new_path);
        free(path_new);
        return 1;
    }
    if (file_link == FILE_LINK_HARD)
    {
        sprintf(path_new, "h_%s", input_new_path);
        ret = link(input_old_path, path_new);
        free(path_new);
        if (ret)
        {
            if(errno == EEXIST)
            {
                printf("Path link output exist\r\n");
            }
            printf("%s:Error when create hardlink - errno: %d\r\n", __FUNCTION__, errno);
            return 1;
        }
    }
    else if (file_link == FILE_LINK_SOFT)
    {
        sprintf(path_new, "s_%s", input_new_path);
        ret = symlink(input_old_path, path_new);
        free(path_new);
        if (ret)
        {
            if(errno == EEXIST)
            {
                printf("Path link output exist\r\n");
            }
            printf("%s:Error when create soft link - errno: %d\r\n", __FUNCTION__, errno);
            return 1;
        }
    }
    else
    {
        printf("%s:Invalid link operation\r\n", __FUNCTION__);
        free(path_new);
        return 1;
    }
    return 0;
}
