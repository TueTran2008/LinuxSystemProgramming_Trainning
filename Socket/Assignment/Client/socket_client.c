#include <netdb.h>
#include "socket_client.h"
#include <signal.h>


static struct addrinfo m_server_info;

int main(int argc, char *argv[])
{
    int cfd;
    ssize_t num_read;
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
        printf("%s server-host [sequence-len]\n", argv[0]);

    /* Call getaddrinfo() to obtain a list of addresses that
       we can try connecting to */

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_family = AF_UNSPEC;                /* Allows IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)    
        printf("signal");

    if (getaddrinfo(argv[1], PORT_NUM, &hints, &result) != 0)
        printf("getaddrinfo");

    /* Walk through returned list until we find an address structure
       that can be used to successfully connect a socket */

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        cfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (cfd == -1)
            continue;                           /* On error, try next address */

        if (connect(cfd, rp->ai_addr, rp->ai_addrlen) != -1)
        {
            memcpy(&m_server_info, rp, sizeof(struct addrinfo));
            break;  
        }
                                        /* Success */

        /* Connect failed: close this socket and try next address */

        close(cfd);
    }

    if (rp == NULL)
        printf("Could not connect socket to any address");

    freeaddrinfo(result);

    while(1)
    {
        char input[100];
        int ret = scanf("%s", input);
        printf("My input:%s - len:%d\r\n", input, ret);
        if (write(cfd, input, strlen(input)) !=  strlen(input))
            printf("Partial/failed write (reqLenStr)");
        if (write(cfd, "\n", 1) != 1)
            printf("Partial/failed write (newline)");
        printf("Wait to read\r\n");
        /* Read and display sequence number returned by server */
        char buffer[100];
        num_read = readLine(cfd, buffer, sizeof(buffer));
        if (num_read == -1)
        {
            printf("readLine");
            continue;
        }
        if (num_read == 0)
        {
            printf("Unexpected EOF from server");
            continue;
        }
        else
        {
            printf("Response from server: %s - len: %ld\r\n", buffer, num_read);
        }
        cfd = socket(m_server_info.ai_family, m_server_info.ai_socktype, m_server_info.ai_protocol);
        if (cfd == -1)
            continue;  
        if (cfd == -1)
            continue;                           /* On error, try next address */

        if (connect(cfd, m_server_info.ai_addr, m_server_info.ai_addrlen) == -1)
        {
            printf("Error when reconnect socket\r\n");
        }

    }
    exit(EXIT_SUCCESS);                         /* Closes 'cfd' */
}
