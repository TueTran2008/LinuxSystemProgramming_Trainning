#ifndef __FILE_LINK_H__
#define __FILE_LINK_H__

typedef enum
{
    FILE_LINK_HARD,
    FILE_LINK_SOFT,
    FILE_LINK_MAX
}file_link_t;
int file_link_create(file_link_t file_link, char *input_old_path, char *input_new_path);
#endif