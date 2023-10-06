/******************************************************************************
 *                              HEADER AND DIRECTORY 
******************************************************************************/
#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <netdb.h>
#include <stdlib.h>
#include "socket_server.h"
#include <pthread.h>
#include "llist.h"

/******************************************************************************
 *                              DEFINE AND TYPEDEF 
******************************************************************************/
#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)
#define BACKLOG 50
/******************************************************************************
 *                              PRIVATE FUNCTION 
******************************************************************************/
/**
 * @brief Periodic thread perform delete each conenction thrread
 * @param arg Pointer point to argument 
 */
static void *delete_thread(void *arg)
{
    pthread_t thread_self = pthread_self();
    printf("thread id:%ld\r\n", thread_self);
    llist *l_thread_list = (llist*)arg;
    while (1)   /*Infinity loops delete all socket thread*/
    {
        if (l_thread_list == NULL || *l_thread_list == NULL) 
        {
            //printf("llist_traverse: list is null\n");
            continue;
        }
        else
        {
            struct node *curr = *l_thread_list;
            int node_index = 0;
            // Traverse the list and perform the operation on each node
            while (curr != NULL) 
            {
                if(curr->data != NULL)
                {
                    ss_thread_data_t* thread_data = (ss_thread_data_t*)curr->data;
                    pthread_t thread_id = thread_data->thread_id;
                    if (thread_self != thread_id)
                    {
                        if (node_index)
                        {
                            int ret = pthread_tryjoin_np(thread_id, NULL);
                            if (ret == 0)
                            {
                                printf("Delete thread id %ld\r\n", thread_id);
                                llist_delete_at_position(l_thread_list, node_index);
                                free(curr->data);
                                break;
                            }
                        }
                    }
                    curr = curr->next;
                    node_index++;
                }
            }
        }
    }
    return NULL;
}
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
/**
 * @brief Connection thread perform read and write function
 * @param arg Pointer point to argument 
 */
static void *thread_func(void *arg)
{
    char buffer[ADDRSTRLEN];
    ss_thread_data_t thread_data = *(ss_thread_data_t *)arg;
    if (getnameinfo((struct sockaddr *) &thread_data.cl_addr, thread_data.sock_len, thread_data.host, NI_MAXHOST, thread_data.service, NI_MAXSERV, 0) == 0)
    {
        snprintf(buffer, ADDRSTRLEN, "(%s, %s)", thread_data.host, thread_data.service);
    }
    else
    {
        snprintf(buffer, ADDRSTRLEN, "(?UNKNOWN?)");
    }
    printf("Connection from %s\n", buffer);
    while (1)
    {
            /* Read client request, send sequence number back */
        if (read_line(thread_data.socket_fd, buffer, INT_LEN) <= 0) 
        {
            close(thread_data.socket_fd);
            continue;                   /* Failed read; skip request */
        }
        reverse_socket_string(buffer);
        if (write(thread_data.socket_fd, buffer, strlen(buffer)) != strlen(buffer))
        {
            fprintf(stderr, "Error on write");
        }
        if (close(thread_data.socket_fd) == -1)           /* Close connection */
        {
            printf("close");
        }
        return NULL;
    }
}
/******************************************************************************
 *                              GLoBAL FUNCTION 
******************************************************************************/
void socket_server(char *p_input_hostname)
{
    uint32_t seqNum;        
    struct sockaddr_storage claddr;
    int lfd, cfd, optval, reqLen;
    socklen_t addrlen;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int ret;
    static pthread_t delete_socket_thread;
    if(p_input_hostname == NULL)
    {
        printf("Please specify host name\r\n");
        exit(0);
    }

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

    if (getaddrinfo(p_input_hostname, PORT_NUM, &hints, &result) != 0)
        printf("getaddrinfo");

    /* Walk through returned list until we find an address structure
       that can be used to successfully create and bind a socket */
    optval = 1;
    for (rp = result; rp != NULL; rp = rp->ai_next) 
    {
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
    llist *thread_list = llist_create(NULL);
    ret = pthread_create(&delete_socket_thread, NULL, delete_thread, (void*)thread_list);
    llist_insert_at_end(thread_list, &delete_socket_thread);
    printf("Insert thread %ld to list\r\n", delete_socket_thread);
    if (ret != 0)
    {
        printf("Error when create delete thread\r\n");
        exit(0);
    }
    for (;;) 
    {   
        /* Handle clients iteratively */
        /* Accept a client connection, obtaining client's address */
        addrlen = sizeof(struct sockaddr_storage);
        cfd = accept(lfd, (struct sockaddr *) &claddr, &addrlen);
        if (cfd == -1) 
        {
            printf("accept");
        }
        else
        {
            ss_thread_data_t *p_new_thread = (ss_thread_data_t*)malloc(sizeof(ss_thread_data_t));
            p_new_thread->cl_addr = claddr;
            p_new_thread->sock_len = addrlen;
            p_new_thread->socket_fd = cfd;
            ret = pthread_create(&p_new_thread->thread_id, NULL, thread_func, p_new_thread);
            if(ret != 0)
            {
                printf("Cannot create thread\r\n");
                continue;
            }
            printf("Insert thread %ld to list\r\n", p_new_thread->thread_id);
            llist_insert_at_end(thread_list, p_new_thread);
        }
        int ret = pthread_tryjoin_np(delete_socket_thread, NULL);
        if (ret == 0)
        {
            printf("We shold never reach this\r\n");
            exit(0);
        }
    }
}
