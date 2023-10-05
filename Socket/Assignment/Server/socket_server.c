#include <netdb.h>
#include <stdlib.h>
#include "socket_server.h"
#include <pthread.h>



#define BACKLOG 50

/*!
 * @brief           Reverse string with the lawt newline character
 * @param[in]       p_string_in Pointer to input string
 */
static void reverse_socket_string(char *p_string_in)
{
    int input_string_len = strlen(p_string_in);
    char reverse_buf[input_string_len];
    memset(reverse_buf, 0, sizeof(reverse_buf));
    if (p_string_in == NULL)
    {
        return;
    }
    memcpy(reverse_buf, p_string_in, input_string_len - 1);
    reverse_buf[input_string_len - 1] = 0;
    utilities_reverse_string(reverse_buf);
    sprintf(p_string_in, "%s\n", reverse_buf);
}

int main(int argc, char *argv[])
{
    uint32_t seqNum;
    char read_buffer[INT_LEN];            /* Length of requested sequence */
    //char client_buff[INT_LEN*2];            /* Start of granted sequence */
    struct sockaddr_storage claddr;
    int lfd, cfd, optval, reqLen;
    socklen_t addrlen;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)
    char addrStr[ADDRSTRLEN];
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    if (argc > 1 && strcmp(argv[1], "--help") == 0)
        printf("%s [server-name]\n", argv[0]);

    seqNum = (argc > 1) ? atoi(argv[1]) : 0;

    /* Ignore the SIGPIPE signal, so that we find out about broken connection
       errors via a failure from write(). */

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)    
        printf("signal");

    /* Call getaddrinfo() to obtain a list of addresses that
       we can try binding to */

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;        /* Allows IPv4 or IPv6 */
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
                        /* Wildcard IP address; service name is numeric */

    if (getaddrinfo(argv[1], PORT_NUM, &hints, &result) != 0)
        printf("getaddrinfo");

    /* Walk through returned list until we find an address structure
       that can be used to successfully create and bind a socket */
    optval = 1;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        lfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (lfd == -1)
            continue;                   /* On error, try next address */

        if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))
                == -1)
             printf("setsockopt");

        if (bind(lfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;                      /* Success */

        /* bind() failed: close this socket and try next address */

        close(lfd);
    }

    if (rp == NULL)
        printf("Could not bind socket to any address");

    if (listen(lfd, BACKLOG) == -1)
        printf("listen");

    freeaddrinfo(result);

    for (;;) {                  /* Handle clients iteratively */

        /* Accept a client connection, obtaining client's address */

        addrlen = sizeof(struct sockaddr_storage);
        cfd = accept(lfd, (struct sockaddr *) &claddr, &addrlen);
        if (cfd == -1) 
        {
            printf("accept");
            sleep(1);
            continue;
        }
        if (getnameinfo((struct sockaddr *) &claddr, addrlen,
                    host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
            snprintf(addrStr, ADDRSTRLEN, "(%s, %s)", host, service);
        else
            snprintf(addrStr, ADDRSTRLEN, "(?UNKNOWN?)");
        printf("Connection from %s\n", addrStr);

        /* Read client request, send sequence number back */

        if (readLine(cfd, read_buffer, INT_LEN) <= 0) {
            close(cfd);
            continue;                   /* Failed read; skip request */
        }
        reverse_socket_string(read_buffer);

        if (write(cfd, read_buffer, strlen(read_buffer)) != strlen(read_buffer))
            fprintf(stderr, "Error on write");

        if (close(cfd) == -1)           /* Close connection */
            printf("close");
    }
}
