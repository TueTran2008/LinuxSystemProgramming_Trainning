/******************************************************************************
 *                              HEADER AND DIRECTORY 
******************************************************************************/
#define _DEFAULT_SOURCE /* Get major() and minor() from <sys/types.h> */
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "file_type.h"
#if(DEBUG_FILE_TYPE_ENABLE)
#define DEBUG_FILE_TYPE(format_, ...)  printf(format_, ##__VA_ARGS__)
#else
#define DEBUG_FILE_TYPE(format_, ...) (void)(0)
#endif
/******************************************************************************
 *                              GLOABAL FUNCTION 
******************************************************************************/
int file_type_list_file(const char* input_dir, char p_out[][256], int *number_of_file)
{
    if (input_dir == NULL)
    {
        printf("%s Input path is NULL\r\n", __FUNCTION__);
        return 1;
    }
    DIR *dirp;
    struct dirent *dp;
    int number_of_found_find = 0;
    /*Open directory*/
    dirp = opendir(input_dir);
    if (dirp == NULL)
    {
        printf("%s:Open directory fail on %s", __FUNCTION__, input_dir);
        return 1;
    }
    else
    {
        DEBUG_FILE_TYPE("Open directory %s successfully\r\n", input_dir);
    }
    /*Read all file in directory*/
    while(1)
    {
        errno = 0; 
        dp = readdir(dirp);
        if (dp == NULL)
        {
            break;
        }
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue; /* Skip . and .. */
        DEBUG_FILE_TYPE("%s\n", dp->d_name);
        sprintf(&p_out[number_of_found_find][0], "%s", dp->d_name);
        number_of_found_find++;
        if (number_of_found_find > FILE_MAX_IN_FOLDER)
        {
            printf("Max number of file in folder: %d\r\n", FILE_MAX_IN_FOLDER);
            return 1;
        }
    }
    if (errno != 0)
    {
        printf("readdir");
        return 1;
    }
    if (closedir(dirp) == -1)
    {
        printf("Error on close directory stream\r\n");
        return 1;
    }
    *number_of_file = number_of_found_find;
    DEBUG_FILE_TYPE("%s Found %d files\n", __FUNCTION__, *number_of_file);
    return 0;
}

void file_type_display_type(const struct stat *sb)
{
 printf("File type: ");
 switch (sb->st_mode & S_IFMT) 
 {
    case S_IFREG: 
        printf("Regular file\n"); 
        break;
    case S_IFDIR: 
        printf("directory\n");
        break;
    case S_IFCHR: 
        printf("character device\n"); 
        break;
    case S_IFBLK: 
        printf("block device\n"); 
        break;
    case S_IFLNK: 
        printf("symbolic (soft) link\n"); 
        break;
    case S_IFIFO: 
        printf("FIFO or pipe\n"); 
        break;
    case S_IFSOCK: 
        printf("socket\n"); 
        break;
    default: 
        printf("unknown file type?\n"); 
        break;
 }
#if(!DISPLAY_ONLY_FILE_TYPE)
 printf("Device containing i-node: major=%ld minor=%ld\n",
 (long) major(sb->st_dev), (long) minor(sb->st_dev));

 printf("I-node number: %ld\n", (long) sb->st_ino);
 printf("Mode: %lo (%s)\n", (unsigned long) sb->st_mode, filePermStr(sb->st_mode, 0));
 if (sb->st_mode & (S_ISUID | S_ISGID | S_ISVTX))
 {
    printf(" special bits set: %s%s%s\n",
    (sb->st_mode & S_ISUID) ? "set-UID " : "",
    (sb->st_mode & S_ISGID) ? "set-GID " : "",
    (sb->st_mode & S_ISVTX) ? "sticky " : "");
 }
 printf("Number of (hard) links: %ld\n", (long) sb->st_nlink);
 printf("Ownership: UID=%ld GID=%ld\n", (long) sb->st_uid, (long) sb->st_gid);
 if (S_ISCHR(sb->st_mode) || S_ISBLK(sb->st_mode))
 {
    printf("Device number (st_rdev): major=%ld; minor=%ld\n", (long) major(sb->st_rdev), (long) minor(sb->st_rdev));
 }
 printf("File size: %lld bytes\n", (long long) sb->st_size);
 printf("Optimal I/O block size: %ld bytes\n", (long) sb->st_blksize);
 printf("512B blocks allocated: %lld\n", (long long) sb->st_blocks);
#endif
}