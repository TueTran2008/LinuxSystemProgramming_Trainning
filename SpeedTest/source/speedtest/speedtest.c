/**
 * @file speedtest.c
 * @brief Implementation of speed test functionalities.
 */
#include "speedtest.h"
#include <pthread.h>
#include <stdlib.h>
#include "app_debug.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include "speedtest_utilities.h"
#include "socket_ip.h"
#include "socket_http.h"
#include "socket_utilities.h"
/*Supprot HTTPS protocol*/
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>


/**
 * @brief Macro for verbose debugging messages.
 */
#if (DEBUG_LEVEL_VERBOSE >= DEBUG_SPEEDTEST_LEVEL)
#define DEBUG_SPEEDTEST_VERBOSE(format_, ...)  DEBUG_VERBOSE(format_, ##__VA_ARGS__)
#else
#define DEBUG_SPEEDTEST_VERBOSE(format_, ...) (void)(0)
#endif
/**
 * @brief Macro for info-level debugging messages.
 */
#if (DEBUG_LEVEL_INFO >= DEBUG_SPEEDTEST_LEVEL)
#define DEBUG_SPEEDTEST_INFO(format_, ...)  DEBUG_INFO(format_, ##__VA_ARGS__)
#else
#define DEBUG_SPEEDTEST_INFO(format_, ...) (void)(0)
#endif
/**
 * @brief Macro for error-level debugging messages.
 */
#if (DEBUG_LEVEL_ERROR >= DEBUG_SPEEDTEST_LEVEL)
#define DEBUG_SPEEDTEST_ERROR(format_, ...)  DEBUG_ERROR(format_, ##__VA_ARGS__)
#else
#define DEBUG_SPEEDTEST_ERROR(format_, ...) (void)(0)
#endif
/**
 * @brief Macro for raw debugging messages.
 */
#if (DEBUG_LEVEL_RAW >= DEBUG_SPEEDTEST_LEVEL)
#define DEBUG_SPEEDTEST_RAW(format_, ...)  DEBUG_RAW(format_, ##__VA_ARGS__)
#else
#define DEBUG_SPEEDTEST_RAW(format_, ...) (void)(0)
#endif

/**
 * @brief Size of the upload buffer.
 */
#define UL_BUFFER_SIZE 8192
/**
 * @brief Number of times to repeat the upload buffer.
 */
#define UL_BUFFER_TIMES 10240
/**
 * @brief Size of the download buffer.
 */
#define DL_BUFFER_SIZE 8192


static float start_dl_time, stop_dl_time, start_ul_time, stop_ul_time;

static st_thread_data_t *thread_data = NULL;

static pthread_mutex_t pthread_mutex = PTHREAD_MUTEX_INITIALIZER; 

long int total_dl_size = 0, total_ul_size = 0;

static int thread_all_stop = 0;

/**
 * @brief Handles SIGALRM signal to stop all threads.
 *
 * @param signo Signal number.
 */
static void speedtest_stop_all_thread(int signo) 
{
    if (signo == SIGALRM) 
    {
        thread_all_stop = 1;
    }
    return;
}
/**
 * @brief Download thread function.
 *
 * @param arg Pointer to thread data.
 * @return NULL.
 */
static void *download_thread(void *arg) 
{
    st_thread_data_t *t_arg = (st_thread_data_t*)arg;
    int i = t_arg->thread_index;

    int fd;
    char sbuf[315] = {0}, rbuf[DL_BUFFER_SIZE];
    struct timeval tv;
    fd_set fdSet;

    int ret = 0;
    SSL *ssl = NULL;
    SSL_CTX *ctx = NULL;
    SSL_METHOD *client_method = NULL;
    X509 *server_cert = NULL;
    // char *str, *host_name, output_buf[4096], input_buf[4096], host_header[512];
    struct hostent *host_entry = NULL;
    struct sockaddr_in server_socket_address;
    struct in_addr ip;    
    char *x509_str;
    int count = 0;
    ssize_t size = 0;

    if ((fd = socket(t_arg->servinfo.ai_family, SOCK_STREAM, 0)) == -1) 
    {    
        DEBUG_SPEEDTEST_ERROR("Open socket error! - ernno: %d\r\n\n");
        sleep(5);
        goto err;
    }
    if (connect(fd, (struct sockaddr *)t_arg->servinfo.ai_addr, t_arg->servinfo.ai_addrlen) == -1) {
        DEBUG_SPEEDTEST_ERROR("Domain name: %s - Socket connect error! - errno:%s\r\n", t_arg->domain_name, strerror(errno));
        sleep(1);
        goto err;
    }
    sprintf(sbuf,
            "GET /%s HTTP/1.0\r\n"
            "Host: %s\r\n"
            "User-Agent: status\r\n"
            "Accept: */*\r\n\r\n", t_arg->request_url, t_arg->domain_name);
    if(t_arg->protocol == SPEEDTEST_SERVER_PROTOCOL_HTTPS)
    {
        SSLeay_add_ssl_algorithms();
        client_method = (SSL_METHOD*)SSLv23_client_method();
        SSL_load_error_strings();
        ctx = SSL_CTX_new(client_method); 
        if(ctx == NULL)
        {
            DEBUG_SPEEDTEST_ERROR("Cannot create certificate ctx\r\n");
        }
        DEBUG_SPEEDTEST_VERBOSE("(1) SSL context initialized\n\n");
        ssl = SSL_new(ctx); /* create SSL stack endpoint*/
        if(ssl == NULL)
        {
            DEBUG_SPEEDTEST_ERROR("Cannot create certificate ssl\r\n");
        }

        SSL_set_fd(ssl, fd); /* attach SSL stack to socket
        */
        ret = SSL_connect(ssl); /* initiate SSL handshake */ 
        DEBUG_SPEEDTEST_VERBOSE("(4) SSL endpoint created & handshake completed\n\n");
        printf("(5) SSL connected with cipher: %s\n\n",
        SSL_get_cipher(ssl));
        if(SSL_get_cipher(ssl) == NULL)
        {
            DEBUG_SPEEDTEST_VERBOSE("Cannot get cirpher \r\n");
        }
        if(server_cert == NULL)
        {
            DEBUG_SPEEDTEST_VERBOSE("Cannot get cirpher X509 \r\n");
            return NULL;
        }

        DEBUG_SPEEDTEST_VERBOSE("(6) server's certificate was received:\n\n");

        x509_str = X509_NAME_oneline(X509_get_subject_name(server_cert), 0, 0);
        DEBUG_SPEEDTEST_VERBOSE(" subject: %s\n", x509_str);

        x509_str = X509_NAME_oneline(X509_get_issuer_name(server_cert), 0, 0);
        DEBUG_SPEEDTEST_VERBOSE(" issuer: %s\n\n", x509_str);

        /* certificate verification would happen here */

        X509_free(server_cert);
    }
    /*Send the HTTP header*/
    if (t_arg->protocol == SPEEDTEST_SERVER_PROCOTOL_HTTP)
    {
        if ((size = send(fd, sbuf, strlen(sbuf), 0)) != strlen(sbuf)) 
        {
            DEBUG_SPEEDTEST_ERROR(":%s: HTTP Can't send header to server\n", __FUNCTION__);
            goto err;
        }
    }
    else if (t_arg->protocol == SPEEDTEST_SERVER_PROTOCOL_HTTPS)
    {
        if (size = SSL_write(ssl, sbuf, strlen(sbuf)) != strlen(sbuf))
        {
            DEBUG_SPEEDTEST_ERROR(":%s HTTPS Can't send header to server\n", __FUNCTION__);
            goto err;
        }
    }
    else 
    {
        DEBUG_ERROR("Invalid protocol\r\n");
    }
    DEBUG_SPEEDTEST_VERBOSE("%s: HTTP Get message: %s\r\n", __FUNCTION__, sbuf);
    if (t_arg->protocol == SPEEDTEST_SERVER_PROCOTOL_HTTP)
    {
        while(1) 
        {
            /*None Blocking API*/
            FD_ZERO(&fdSet);
            FD_SET(fd, &fdSet);

            tv.tv_sec = 3;
            tv.tv_usec = 0;
            int status = select(fd + 1, &fdSet, NULL, NULL, &tv);                                                         

            int recv_byte = recv(fd, rbuf, sizeof(rbuf), 0);
            if  (status > 0 && FD_ISSET(fd, &fdSet)) 
            {
                if(recv_byte < 0) 
                {
                    printf("Can't receive data!\n");
                    break;
                } 
                else if(recv_byte == 0)
                {
                    break;
                } 
                else 
                {
                    pthread_mutex_lock(&pthread_mutex);
                    total_dl_size += recv_byte;
                    pthread_mutex_unlock(&pthread_mutex);
                }
                if (thread_all_stop)
                {
                    break;
                }
            }   
        }
    }
    else if (t_arg->protocol == SPEEDTEST_SERVER_PROTOCOL_HTTPS)
    {
        while (1)
        {
            int recv_byte = SSL_read(ssl, sbuf, sizeof(sbuf));
            if (recv_byte > 0)
            {
                pthread_mutex_lock(&pthread_mutex);
                total_dl_size += recv_byte;
                pthread_mutex_unlock(&pthread_mutex);
            }
            else 
            {
                count++;
                ret = SSL_get_error(ssl, recv_byte);
                switch(ret)
                {
                    case SSL_ERROR_NONE:
                    {
                        DEBUG_SPEEDTEST_ERROR("SSL_ERROR_NONE %i\r\n", count);
                    }
                        break;
                    case SSL_ERROR_ZERO_RETURN:
                    {
                        fd_set fds;
                        DEBUG_SPEEDTEST_ERROR("SSL_ERROR_WANT_READ :%i\r\n", count);
                        int sock_fd = SSL_get_rfd(ssl);
                        FD_ZERO(&fds);
                        FD_SET(sock_fd, &fds);
                        tv.tv_sec = 3;
                        tv.tv_usec = 0;
                        ret = select(sock_fd + 1, NULL, &fds, NULL, &tv);
                        if (ret > 0 && FD_ISSET(fd, &fdSet))
                        {
                            /*Back to read data*/
                            continue; // can write more data now...
                        }

                        if (ret == 0) 
                        {
                            // timeout...
                            break;
                        } 
                        else 
                        {
                            // error...
                            DEBUG_SPEEDTEST_VERBOSE("Cannot receive data\r\n");
                            break;
                        }
                    }
                        break;
                    default:
                    {
                        DEBUG_SPEEDTEST_ERROR("error %i:%i\n", recv_byte, ret); 
                        break;
                    }
                }
            }
            if (thread_all_stop)
            {
                break;
            }
        }
    }

err: 
    if (t_arg->protocol == SPEEDTEST_SERVER_PROTOCOL_HTTPS)
    {
        SSL_shutdown(ssl);
        SSL_free (ssl);
        SSL_CTX_free (ctx);
    }
    if(fd) 
    {
        close(fd);
    }
    t_arg->running = 0;
    return NULL;
}
/**
 * @brief Upload thread function.
 *
 * @param arg Pointer to thread data.
 * @return NULL.
 */
static void *upload_thread(void *arg) 
{
    int fd;
    char data[UL_BUFFER_SIZE], sbuf[512];
    int i, j, size = 0;
    struct timeval tv;
    fd_set fdSet;

    int ret = 0;
    SSL *ssl = NULL;
    SSL_CTX *ctx = NULL;
    SSL_METHOD *client_method = NULL;
    X509 *server_cert = NULL;
    // char *str, *host_name, output_buf[4096], input_buf[4096], host_header[512];
    struct hostent *host_entry = NULL;
    struct in_addr ip;    
    char *x509_str;
    int count = 0;


    st_thread_data_t *t_arg = (st_thread_data_t*)arg;
    i = t_arg->thread_index;

    memset(data, 0, sizeof(char) * UL_BUFFER_SIZE);

    if((fd = socket(t_arg->servinfo.ai_family, SOCK_STREAM, 0)) == -1) 
    {      
        sleep(1);                                     
        DEBUG_SPEEDTEST_ERROR("Open socket error!\n");
        goto err;
    }
    if (connect(fd, (struct sockaddr *)t_arg->servinfo.ai_addr, t_arg->servinfo.ai_addrlen) == -1)
    {
        DEBUG_SPEEDTEST_ERROR("Domain name:%s - Socket connect error! - errno: %s\r\n", t_arg->domain_name, strerror(errno));
        sleep(1);
        goto err;
    }
    if(t_arg->protocol == SPEEDTEST_SERVER_PROTOCOL_HTTPS)
    {
        SSLeay_add_ssl_algorithms();
        client_method = (SSL_METHOD*)SSLv23_client_method();
        SSL_load_error_strings();
        ctx = SSL_CTX_new(client_method); 
        if(ctx == NULL)
        {
            DEBUG_SPEEDTEST_ERROR("Cannot create certificate ctx\r\n");
        }
        DEBUG_SPEEDTEST_VERBOSE("(1) SSL context initialized\n\n");
        ssl = SSL_new(ctx); /* create SSL stack endpoint*/
        if(ssl == NULL)
        {
            DEBUG_SPEEDTEST_ERROR("Cannot create certificate ssl\r\n");
        }

        SSL_set_fd(ssl, fd); /* attach SSL stack to socket
        */
        ret = SSL_connect(ssl); /* initiate SSL handshake */ 
        DEBUG_SPEEDTEST_VERBOSE("(4) SSL endpoint created & handshake completed\n\n");
        printf("(5) SSL connected with cipher: %s\n\n",
        SSL_get_cipher(ssl));
        if(SSL_get_cipher(ssl) == NULL)
        {
            DEBUG_SPEEDTEST_VERBOSE("Cannot get cirpher \r\n");
        }
        if(server_cert == NULL)
        {
            DEBUG_SPEEDTEST_VERBOSE("Cannot get cirpher X509 \r\n");
            return NULL;
        }

        DEBUG_SPEEDTEST_VERBOSE("(6) server's certificate was received:\n\n");

        x509_str = X509_NAME_oneline(X509_get_subject_name(server_cert), 0, 0);
        DEBUG_SPEEDTEST_VERBOSE(" subject: %s\n", x509_str);

        x509_str = X509_NAME_oneline(X509_get_issuer_name(server_cert), 0, 0);
        DEBUG_SPEEDTEST_VERBOSE(" issuer: %s\n\n", x509_str);

        /* certificate verification would happen here */

        X509_free(server_cert);
    }
    sprintf(sbuf,
            "POST /%s HTTP/1.0\r\n"
            "Content-type: application/x-www-form-urlencoded\r\n"
            "Host: %s\r\n"
            "Content-Length: %ld\r\n\r\n", t_arg->request_url, t_arg->domain_name, sizeof(data) * UL_BUFFER_TIMES);


    if (t_arg->protocol == SPEEDTEST_SERVER_PROCOTOL_HTTP)
    {
        if ((size = send(fd, sbuf, strlen(sbuf), 0)) != strlen(sbuf)) 
        {
            DEBUG_SPEEDTEST_ERROR("HTTP Can't send header to server\n");
            goto err;
        }
    }
    else if (t_arg->protocol == SPEEDTEST_SERVER_PROTOCOL_HTTPS)
    {
        if (size = SSL_write(ssl, sbuf, strlen(sbuf)) != strlen(sbuf))
        {
            DEBUG_SPEEDTEST_ERROR("HTTPS Can't send header to server\n");
            goto err;
        }
    }
    else 
    {
        DEBUG_ERROR("Invalid protocol\r\n");
    }

    pthread_mutex_lock(&pthread_mutex);
    total_ul_size += size;
    pthread_mutex_unlock(&pthread_mutex);

    for (j = 0; j < UL_BUFFER_TIMES; j++) 
    {

        if ((size = send(fd, data, sizeof(data), 0)) != sizeof(data)) 
        {
            DEBUG_SPEEDTEST_ERROR("%s: HTTP Can't upload data to server - size: %ld\n", __FUNCTION__, size);
            goto err;
        }
        pthread_mutex_lock(&pthread_mutex);
        total_ul_size += size;
        pthread_mutex_unlock(&pthread_mutex);
        if(thread_all_stop)
            goto err;
    }
    if (t_arg->protocol == SPEEDTEST_SERVER_PROCOTOL_HTTP)
    {
        while(1) 
        {
            FD_ZERO(&fdSet);
            FD_SET(fd, &fdSet);

            tv.tv_sec = 3;
            tv.tv_usec = 0;
            int status = select(fd + 1, &fdSet, NULL, NULL, &tv);                                                         

            int recv_byte = recv(fd, sbuf, sizeof(sbuf), 0);
            if(status > 0 && FD_ISSET(fd, &fdSet)) 
            {
                if(recv_byte < 0) 
                {
                    printf("Can't receive data!\n");
                    break;
                } 
                else if (recv_byte == 0)
                {
                    break;
                }
            }   
        }
    }
    else if (t_arg->protocol == SPEEDTEST_SERVER_PROTOCOL_HTTPS)
    {
        while (1)
        {
            int recv_byte = SSL_read(ssl, sbuf, sizeof(sbuf));
            if (recv_byte > 0)
            {

            }
            else 
            {
                count++;
                ret = SSL_get_error(ssl, recv_byte);
                switch(ret)
                {
                    case SSL_ERROR_NONE:
                    {
                        DEBUG_SPEEDTEST_ERROR("SSL_ERROR_NONE %i\r\n", count);
                    }
                        break;
                    case SSL_ERROR_ZERO_RETURN:
                    {
                        fd_set fds;
                        DEBUG_SPEEDTEST_ERROR("SSL_ERROR_WANT_READ :%i\r\n", count);
                        int sock_fd = SSL_get_rfd(ssl);
                        FD_ZERO(&fds);
                        FD_SET(sock_fd, &fds);
                        tv.tv_sec = 3;
                        tv.tv_usec = 0;
                        ret = select(sock_fd + 1, NULL, &fds, NULL, &tv);
                        if (ret > 0)
                            continue; // can write more data now...

                        if (ret == 0) 
                        {
                            // timeout...
                            break;
                        } 
                        else 
                        {
                            // error...
                            DEBUG_SPEEDTEST_VERBOSE("Cannot receive data\r\n");
                            break;
                        }
                    }
                        break;
                    default:
                    {
                        DEBUG_SPEEDTEST_ERROR("error %i:%i\n", recv_byte, ret); 
                        break;
                    }
                }
            }
        }
    }
err: 
    if (t_arg->protocol == SPEEDTEST_SERVER_PROTOCOL_HTTPS)
    {
        SSL_shutdown(ssl);
        //close (fd);
        SSL_free (ssl);
        SSL_CTX_free (ctx);
    }
    if (fd) 
        close(fd);
    t_arg->running = 0;

    return NULL;
}
/**
 * @brief Initiates speed test download.
 *
 * @param nearest_server Pointer to the nearest server data.
 * @param number_of_thread Number of threads to use for download.
 * @return 1 on success, -1 on failure.
 */
int speedtest_download(server_data_t *nearest_server, unsigned int number_of_thread, st_server_protocol_t protocol) 
{
    const char download_filename[64] = "random3500x3500.jpg";  //23MB
    char url[128]= {0}, request_url[128] = {0}, dummy[128] = {0}, buf[128];
    char *ptr = NULL;
    int i;
    st_thread_data_t *download_thread_data = (st_thread_data_t*)malloc(number_of_thread * sizeof(st_thread_data_t));
    memset(download_thread_data, 0, sizeof(st_thread_data_t) * number_of_thread);
    printf("Speed test download domain name: %s\r\n", nearest_server->domain_name);
    struct itimerval timer_val;
    if (nearest_server == NULL)
    {
        DEBUG_SPEEDTEST_ERROR("%s: Nearest server is NULL\r\n");
        return -1;
    }
    if (number_of_thread == 0)
    {
        DEBUG_SPEEDTEST_ERROR("%s: No nearest server is NULL\r\n");
        return -1;
    }
    if(download_thread == NULL || download_thread_data == NULL)
    {
        DEBUG_SPEEDTEST_ERROR("Cannot allocate memory of download thread\r\n");
        return -1;
    }
    /*Get the URL of the download site*/
    /*speedtesthni2.viettel.vn:8080
        http://speedtesthni2.viettel.vn:8080/speedtest/upload.php
        request_url = speedtest/upload.php
        dummy = speedtesthni2.viettel.vn:8080
    */
    if (protocol == SPEEDTEST_SERVER_PROCOTOL_HTTP)
    {
        sscanf(nearest_server->url, "http://%[^/]/%s", dummy, request_url);
    }
    else if (protocol == SPEEDTEST_SERVER_PROTOCOL_HTTPS)
    {
        sscanf(nearest_server->url, "https://%[^/]/%s", dummy, request_url);
    }
    strncpy(url, request_url, sizeof(request_url));
    memset(request_url, 0, sizeof(request_url));
    ptr = strtok(url, "/");
    while(ptr != NULL) 
    {
        memset(buf, 0, sizeof(buf));
        strncpy(buf, ptr, strlen(ptr));
        //Change file name
        if(strstr(buf, "upload.") != NULL) 
        {
            strcat(request_url, download_filename);
        } 
        else 
        {
            strcat(request_url, buf);
            strcat(request_url, "/");
        }
        ptr = strtok(NULL, "/");
    }
    DEBUG_SPEEDTEST_INFO("%s: request_url:%s - url: %s\r\n", __FUNCTION__, request_url, nearest_server->url);
    start_dl_time = st_utilities_get_uptime();
    signal(SIGALRM, speedtest_stop_all_thread);
    timer_val.it_value.tv_sec = SPEEDTEST_DURATION;
    timer_val.it_value.tv_usec = 0;
    timer_val.it_interval.tv_sec = 0;
    timer_val.it_interval.tv_usec = 0;
    thread_all_stop = 0;
    if (setitimer(ITIMER_REAL, &timer_val, 0) == -1)
    {
        DEBUG_WARN("Erron when set timer - errno: %s\r\n", strerror(errno));
    }
    while(1) 
    {
        for(i = 0; i < number_of_thread; i++) 
        {
            memcpy(&download_thread_data[i].servinfo, &nearest_server->servinfo, sizeof(download_thread_data[i].servinfo));
            memcpy(download_thread_data[i].domain_name, nearest_server->domain_name, sizeof(nearest_server->domain_name));
            memcpy(download_thread_data[i].request_url, request_url, sizeof(request_url));
            download_thread_data[i].protocol = protocol;
            if(download_thread_data[i].running == 0) 
            {
                download_thread_data[i].thread_index = i;
                download_thread_data[i].running = 1;
                pthread_create(&download_thread_data[i].tid, NULL, download_thread, &download_thread_data[i]);
                DEBUG_SPEEDTEST_VERBOSE("Create thread download :%ld\t\n", download_thread_data[i].tid);
            }
        }
        if (thread_all_stop)
        {
            
            stop_dl_time = st_utilities_get_uptime();
            float duration = stop_dl_time - start_dl_time;
            float dl_speed = (double)total_dl_size/1000/1000/duration*8;
            printf("Download speed: %0.2lf Mbps\r\n", dl_speed);
            break;
        }
    }
    for(i = 0; i < number_of_thread; i++) 
    {
        if(pthread_join(download_thread_data[i].tid, NULL) == 0)
        {
            DEBUG_SPEEDTEST_VERBOSE("Download thread delete: %ld\r\n", download_thread_data[i].tid);
        }
        else
        {
            DEBUG_SPEEDTEST_ERROR("Error delete thread delete: %ld\r\n", download_thread_data[i].tid);
        }
    }
    free(download_thread_data);
    return 1;
}
/**
 * @brief Initiates speed test upload.
 *
 * @param test_server Pointer to the nearest server data.
 * @param number_of_thread Number of threads to use for upload.
 * @return 1 on success, -1 on failure.
 */
int speedtest_upload(server_data_t *test_server, unsigned int number_of_thread, st_server_protocol_t protocol) 
{
    int i;
    char dummy[128] = {0}, request_url[128]={0};
    if (protocol == SPEEDTEST_SERVER_PROCOTOL_HTTP)
    {
        sscanf(test_server->url, "http://%[^/]/%s", dummy, request_url);
    }
    else if (protocol == SPEEDTEST_SERVER_PROTOCOL_HTTPS)
    {
        sscanf(test_server->url, "https://%[^/]/%s", dummy, request_url);
    }
    printf("Speed test upload domain name: %s\r\n", test_server->domain_name);
    start_ul_time = st_utilities_get_uptime();
    //pthread_t calculate_thread;
    struct itimerval timer_val;
    if (test_server == NULL)
    {
        DEBUG_SPEEDTEST_ERROR("%s: Nearest server is NULL\r\n");
        return -1;
    }
    if (number_of_thread == 0)
    {
        DEBUG_SPEEDTEST_ERROR("%s: No nearest server is NULL\r\n");
        return -1;
    }
    st_thread_data_t *up_thread_data = (st_thread_data_t*)malloc(number_of_thread * sizeof(st_thread_data_t));
    if(up_thread_data == NULL)
    {
        DEBUG_SPEEDTEST_ERROR("Cannot allocate memory of upload thread\r\n");
        return -1;
    }
    memset(up_thread_data, 0, sizeof(st_thread_data_t) * number_of_thread);
    signal(SIGALRM, speedtest_stop_all_thread);
    timer_val.it_value.tv_sec = SPEEDTEST_DURATION;
    timer_val.it_value.tv_usec = 0;
    timer_val.it_interval.tv_sec = 0;
    timer_val.it_interval.tv_usec = 0;
    thread_all_stop = 0;
    if (setitimer(ITIMER_REAL, &timer_val, 0) == -1)
    {
        DEBUG_WARN("Error when set timer - errno:%s\r\n", strerror(errno));
    }
    while(1) 
    {
        for(i = 0; i < number_of_thread; i++) 
        {
            memcpy(&up_thread_data[i].servinfo, &test_server->servinfo, sizeof(up_thread_data[i].servinfo));
            memcpy(up_thread_data[i].domain_name, test_server->domain_name, sizeof(test_server->domain_name));
            memcpy(up_thread_data[i].request_url, request_url, sizeof(request_url));
            up_thread_data[i].protocol = protocol;
            if(up_thread_data[i].running == 0) 
            {
                up_thread_data[i].thread_index = i;
                up_thread_data[i].running = 1;
                int s = pthread_create(&up_thread_data[i].tid, NULL, upload_thread, &up_thread_data[i]);
                if(s != 0)
                {
                    DEBUG_ERROR("Error when create thread\r\n");
                }
                else
                {
                    DEBUG_SPEEDTEST_VERBOSE("Creat e thread :%ld\r\n", up_thread_data[i].tid);
                }
            }
        }
        if (thread_all_stop)
        {
            DEBUG_SPEEDTEST_VERBOSE("Stop all task\r\n");
            stop_ul_time = st_utilities_get_uptime();
            float duration = stop_ul_time - start_ul_time;
            float ul_speed = (double)total_ul_size/1000/1000/duration*8;
            printf("Upload speed: %0.2lf Mbps\r\n", ul_speed);
            break;
        }
    }
    for (i = 0; i < number_of_thread; i++) 
    {
        DEBUG_SPEEDTEST_VERBOSE("Delete thread upload: %ld\r\n", up_thread_data[i].tid);
        pthread_join(up_thread_data[i].tid, NULL);
    }
    free(up_thread_data);
    return 1;
}

/**
 * @brief Performs a Speedtest on a specific domain name using the given protocol, operation, and number of threads.
 *
 * @param p_domain_name The domain name to perform the Speedtest on.
 * @param protocol The protocol to be used for the Speedtest (e.g., HTTP, HTTPS).
 * @param operation The type of Speedtest operation (e.g., upload, download).
 * @param number_of_thread Number of threads to be used for the Speedtest.
 * @return 0 if the Speedtest is successful, -1 on failure.
 */
int speedtest_test_domain_name(char *p_domain_name, st_server_protocol_t protocol, st_server_operation_t operation, int number_of_thread)
{
    int ret = 0;
    server_data_t server_data;
    memset(&server_data, 0, sizeof(server_data));
    if (p_domain_name == NULL)
    {
        DEBUG_SPEEDTEST_ERROR("Error when get domain name server\r\n");
        return - 1;
    }
    ret = st_utilities_get_server_through_domain_name(p_domain_name, protocol, &server_data);
    if(ret == -1)
    {
        DEBUG_SPEEDTEST_ERROR("%s:Error when get server datar\r\n", __FUNCTION__);
        return -1;
    }
    if(operation == SPEEDTEST_SERVER_OPERATION_UPLOAD)
    {
        return speedtest_upload(&server_data, number_of_thread, protocol);
         
    }
    else if (operation == SPEEDTEST_SERVER_OPERATION_DOWNLOAD)
    {
        return speedtest_download(&server_data, number_of_thread, protocol);
    }
}
/**
 * @brief Performs a Speedtest on the server with the lowest latency using the given protocol, operation, and number of threads.
 *
 * @param protocol The protocol to be used for the Speedtest (e.g., HTTP, HTTPS).
 * @param operation The type of Speedtest operation (e.g., upload, download).
 * @param number_of_thread Number of threads to be used for the Speedtest.
 * @return 0 if the Speedtest is successful, -1 on failure.
 */
int speedtest_test_lowest_latency(st_server_protocol_t protocol, st_server_operation_t operation, int number_of_thread)
{
    int i, best_server_index;
    client_data_t client_data;
    server_data_t nearest_servers[NEAREST_SERVERS_NUM];
    pthread_t pid;
    struct addrinfo servinfo;
    struct itimerval timerVal;
    char operation_buf[24];
    memset(operation_buf, 0, sizeof(operation_buf));
    printf("Process to find the best server...\r\n");
    if (protocol == SPEEDTEST_SERVER_PROCOTOL_HTTP)
    {
        sprintf(operation_buf, "%s", "http");
    }
    else if (protocol == SPEEDTEST_SERVER_PROTOCOL_HTTPS)
    {
        sprintf(operation_buf, "%s", "https");
    }
    
    memset(&client_data, 0, sizeof(client_data_t));
    for (i = 0; i < NEAREST_SERVERS_NUM; i++) 
    {
        memset(&nearest_servers[i], 0, sizeof(server_data_t));
    }
    if (socket_ipv4_get_from_url(SPEEDTEST_DOMAIN_NAME, operation_buf, &servinfo)) 
    {
        if (!socket_http_get_file((struct sockaddr_in *)servinfo.ai_addr, servinfo.ai_addrlen, SPEEDTEST_DOMAIN_NAME, CONFIG_REQUEST_URL, CONFIG_REQUEST_URL)) 
        {
            DEBUG_ERROR("Can't get your IP address information.\n");
            return -1;
        }
    }
    if(socket_ipv4_get_from_url(SPEEDTEST_SERVERS_DOMAIN_NAME, operation_buf, &servinfo)) 
    {
        if(!socket_http_get_file((struct sockaddr_in *)servinfo.ai_addr, servinfo.ai_addrlen, SPEEDTEST_SERVERS_DOMAIN_NAME, SERVERS_LOCATION_REQUEST_URL, SERVERS_LOCATION_REQUEST_URL)) {
            DEBUG_ERROR("Can't get servers list.\n");
            return -1;
        }
    }
    st_utilities_get_ip_address_position(CONFIG_REQUEST_URL, &client_data);
    DEBUG_SPEEDTEST_VERBOSE("============================================\n");
    DEBUG_SPEEDTEST_VERBOSE("Your IP Address : %s\n", client_data.ipAddr);
    DEBUG_SPEEDTEST_VERBOSE("Your ISP        : %s\n", client_data.isp);
    DEBUG_SPEEDTEST_VERBOSE("============================================\n");
    if(st_utilities_get_nearest_server(client_data.latitude, client_data.longitude, nearest_servers) == 0) 
    {
        DEBUG_SPEEDTEST_VERBOSE("Can't get server list.\n"); 
        return -1;
    }
    if((best_server_index = st_utilities_get_best_server(nearest_servers, SPEEDTEST_SERVER_PROCOTOL_HTTP)) != -1) 
    {
        DEBUG_SPEEDTEST_VERBOSE("==========The best server information==========\n");
        DEBUG_SPEEDTEST_VERBOSE("URL: %s - index:%d\n", nearest_servers[best_server_index].url, best_server_index);
        DEBUG_SPEEDTEST_VERBOSE("Name: %s\n", nearest_servers[best_server_index].name);
        DEBUG_SPEEDTEST_VERBOSE("Country: %s\n", nearest_servers[best_server_index].country);
        DEBUG_SPEEDTEST_VERBOSE("===============================================\n");
        if(operation == SPEEDTEST_SERVER_OPERATION_UPLOAD)
        {
            return speedtest_upload(&nearest_servers[best_server_index], number_of_thread, protocol);
            
        }
        else if (operation == SPEEDTEST_SERVER_OPERATION_DOWNLOAD)
        {
            return speedtest_download(&nearest_servers[best_server_index], number_of_thread, protocol);
        }
        else
        {
            DEBUG_SPEEDTEST_ERROR("Invalid operation\r\n");
        }
    }
    else
    {
        DEBUG_SPEEDTEST_ERROR("Cannot get best server\r\n");
    }
    return 0;
}
