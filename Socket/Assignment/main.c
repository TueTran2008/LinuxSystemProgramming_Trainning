#include "socket_server.h"
#include "socket_client.h"
#include "utilities.h"
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc < 2 || strcmp(argv[1], "--help") == 0)
    {
        printf("Socket programming assignment <Mode> <Host name>\r\n");
        printf("1 as Server\r\n2 as Client");
        printf("Example:..mysocket 1 localhost");
    }   
    if(ultilities_is_number(argv[1]) == 0)
    {
        printf("Invalid mode: Please choose between Client: 2 or Server: 1. Enter --help to get more information\r\n");
    }
    int mode = atoi(argv[1]);
    if (mode == 1)
    {
        socket_server(argv[2]);
    }
    else if (mode == 2)
    {
        socket_client(argv[2]);
    }
    else
    {
        printf("Invalid mode: Please choose between Client or Server\r\n");
    }
}