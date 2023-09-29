#include "file_handle.h"
#include "file_link.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    int ret = 0; /*Variable to check return value of function*/
    char *p_file = NULL;
    char *p_link = NULL;
    if (argc < 3 || strcmp(argv[1], "--help") == 0)
    {
        printf("%s:Listing file type name: <Input file> <Output file>\r\n", argv[0]);
        printf("Example: ./file_p2.ext test.txt \"Hello World from Tran Duc Tue\"\r\n");
        return 0;
    }
    ret = file_handle_check_file_exist(argv[1]);
    if (ret == 0)
    {
        printf("Input %s is not a file\r\n", argv[1]);
        return 0;
    }
    p_file = argv[1];
    p_link = argv[2];
    ret = file_link_create(FILE_LINK_HARD, p_file, p_link);
    if(ret == 1)
    {
        printf("Error when create link\r\n");
        return 0;
    }
    ret = file_link_create(FILE_LINK_SOFT, p_file, p_link);
    if(ret == 1)
    {
        printf("Error when create link\r\n");
        return 0;
    }
}