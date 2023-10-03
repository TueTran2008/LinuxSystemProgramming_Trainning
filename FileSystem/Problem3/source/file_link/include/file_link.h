#ifndef __FILE_LINK_H__
#define __FILE_LINK_H__

typedef enum
{
    FILE_LINK_HARD,
    FILE_LINK_SOFT,
    FILE_LINK_MAX
}file_link_t;

/**
 * @brief Create link to file
 * 
 * @param[in] file_link Can be hardlink or softlink
 * @param[in] input_old_path Pointer to string of old file we want to create link
 * @param[in] input_new_path Pointer to string of the new link file locates
 * @return 0 if success
 *         1 if fail
 */
int file_link_create(file_link_t file_link, char *input_old_path, char *input_new_path);
#endif