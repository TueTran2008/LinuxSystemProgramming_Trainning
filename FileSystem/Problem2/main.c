#include "file_handle.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    int ret = 0; /*Variable to check return value of function*/
    char *p_file = NULL;
    char *p_write_string = NULL;
    if (argc < 3 || strcmp(argv[1], "--help") == 0)
    {
        printf("%s:Listing file type name: <Input file> <String to be written>\r\n", argv[0]);
        printf("Example: ./file_p2.ext test.txt \"Hello World from Tran Duc Tue\"\r\n");
        return 0;
    }
    ret = file_handle_check_file_exist(argv[1]);
    if (ret == 0)
    {
        printf("Input %s is not a file\r\n", argv[1]);
        return 0;
    }
    if (argv[2] == NULL) /*Maybe this is not necessary, haha*/
    {
        printf("Invalid write value");
        return 0;
    }
    p_file = argv[1];
    p_write_string = argv[2];
    file_handle_write(p_file, p_write_string);
    return 0;
}