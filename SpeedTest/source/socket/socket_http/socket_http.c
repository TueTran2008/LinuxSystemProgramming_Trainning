/**
 * @file socket_http.h
 * @brief Header file containing functions and macros related to HTTP socket operations.
 */
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdbool.h>
#include "app_debug.h"
#include <errno.h>
#include "socket_http.h"
#include "socket_ip.h"
#include "socket_utilities.h"
#include <sys/stat.h>
#include <fcntl.h> 

/*Supprot HTTPS protocol*/
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

/**
 * @brief Macro for verbose debugging messages.
 */
#if (DEBUG_LEVEL_VERBOSE >= DEBUG_SOCKET_HTTP_LEVEL)
#define DEBUG_SOCKET_HTTP_VERBOSE(format_, ...)  DEBUG_VERBOSE(format_, ##__VA_ARGS__)
#else
#define DEBUG_SOCKET_HTTP_VERBOSE(format_, ...) (void)(0)
#endif
/**
 * @brief Macro for info-level debugging messages.
 */
#if (DEBUG_LEVEL_INFO >= DEBUG_SOCKET_HTTP_LEVEL)
#define DEBUG_SOCKET_HTTP_INFO(format_, ...)  DEBUG_INFO(format_, ##__VA_ARGS__)
#else
#define DEBUG_SOCKET_HTTP_INFO(format_, ...) (void)(0)
#endif
/**
 * @brief Macro for error-level debugging messages.
 */
#if (DEBUG_LEVEL_ERROR >= DEBUG_SOCKET_HTTP_LEVEL)
#define DEBUG_SOCKET_HTTP_ERROR(format_, ...)  DEBUG_ERROR(format_, ##__VA_ARGS__)
#else
#define DEBUG_SOCKET_HTTP_ERROR(format_, ...) (void)(0)
#endif
/**
 * @brief Macro for raw debugging messages.
 */
#if (DEBUG_LEVEL_RAW >= DEBUG_SOCKET_HTTP_LEVEL)
#define DEBUG_SOCKET_HTTP_RAW(format_, ...)  DEBUG_RAW(format_, ##__VA_ARGS__)
#else
#define DEBUG_SOCKET_HTTP_RAW(format_, ...) (void)(0)
#endif

#define FILE_DIRECTORY_PATH "/tmp/"


/**
 * @brief Performs an HTTP GET request and saves the response to a file.
 *
 * This function establishes a connection to the specified server and sends an HTTP GET request for
 * the provided `request_url`. It saves the received response to the file specified by `filename`.
 * The function uses non-blocking IO to read data from the server and writes it to the file.
 *
 * @param serv Pointer to a sockaddr_in structure representing the server address.
 * @param domain_name The domain name of the server.
 * @param request_url The URL to request from the server.
 * @param filename The name of the file to save the HTTP response.
 * @return 1 if the HTTP request is successful and the response is saved, 0 otherwise.
 */
int socket_http_get_file(struct sockaddr_in *serv, char *domain_name, char *request_url, char *filename) 
{
    int fd;
    char sbuf[256] = {0}, tmp_path[128] = {0};
    char rbuf[8192];
    struct timeval tv;
    fd_set fdSet;
    FILE *fp = NULL;

    memset(sbuf, 0, sizeof(sbuf));
    memset(tmp_path, 0, sizeof(tmp_path));
    memset(rbuf, 0, sizeof(sbuf));
    DEBUG_SOCKET_HTTP_VERBOSE("%s: Sin family:%d\r\n", __FUNCTION__, serv->sin_family);
    if ((fd = socket(serv->sin_family, SOCK_STREAM, 0)) == -1) 
    {
        DEBUG_SOCKET_HTTP_ERROR("Open socket error!\n");
        if (fd)
        {
            close(fd);
        }
        return 0;
    }
    if (socket_utilities_connect_timeout(fd, (struct sockaddr *)serv) == -1) 
    {
        DEBUG_SOCKET_HTTP_ERROR("%s: Socket connect error!\r\nDomain: %s - errno: %s\r\n", __FUNCTION__, domain_name, strerror(errno));
        if (fd) 
        {
            close(fd);
        }
        return 0;
    }
    sprintf(sbuf,
            "GET /%s HTTP/1.0\r\n"
            "Host: %s\r\n"
            "User-Agent: status\r\n"
            "Accept: */*\r\n\r\n", request_url, domain_name);                                                                 

    if(send(fd, sbuf, strlen(sbuf), 0) != strlen(sbuf)) 
    {
        DEBUG_SOCKET_HTTP_ERROR("Can't send data to server\n");
        if(fd) close(fd);
        return 0;
    }
    DEBUG_SOCKET_HTTP_INFO("%s: HTTP Get:\r\n****************************************\r\n%s\r\n****************************************\r\n", __FUNCTION__, sbuf);
    sprintf(tmp_path, "%s%s", FILE_DIRECTORY_PATH, filename);
    DEBUG_SOCKET_HTTP_INFO("%s: HTTP get directory: %s\r\n", __FUNCTION__, tmp_path);
    fp = fopen(tmp_path, "w");
    
    if(fp == NULL)
    {
        DEBUG_SOCKET_HTTP_ERROR("%s: Unable to open :%s - errno: %s\r\n", __FUNCTION__, tmp_path, strerror(errno));
        return 0;
    }
    /*Perform non blocking IO read*/
    while(1) 
    {
        char *ptr=NULL;
        memset(rbuf, 0, sizeof(rbuf));
        FD_ZERO(&fdSet);
        FD_SET(fd, &fdSet);

        tv.tv_sec = 3;
        tv.tv_usec = 0;
        int status = select(fd + 1, &fdSet, NULL, NULL, &tv);
        int i = recv(fd, rbuf, sizeof(rbuf), 0);
        if (status > 0 && FD_ISSET(fd, &fdSet)) 
        {
            if (i < 0) 
            {
                printf("Can't get http file!\n");
                close(fd);
                fclose(fp);
                return 0;
            } 
            else if (i == 0) 
            {
                break;
            } 
            else 
            {
                if ((ptr = strstr(rbuf, "\r\n\r\n")) != NULL) 
                {
                    ptr += 4;
                    fwrite(ptr, 1, strlen(ptr), fp);
                } 
                else 
                {
                    fwrite(rbuf, 1, i, fp);
                }
            }
        }
    }
    close(fd);
    fclose(fp);
    return 1;
}



int https_test(void)
{
    struct addrinfo servinfo;
    char p_server_name[] = "www.speedtest.net";
    memset(&servinfo, 0, sizeof(servinfo));
    if(socket_ipv4_get_from_url(p_server_name, "https", &servinfo) == 0) 
    {
        printf("Errno when resolse ip - errno:%s\r\n", strerror(errno));
        return -1;
    }
    socket_https_get_file((struct sockaddr_in *)servinfo.ai_addr, p_server_name, p_server_name, p_server_name);
}

int socket_https_get_file(struct sockaddr_in *serv, char *domain_name, char *request_url, char *filename) 
{

    int fd;
    SSL *ssl = NULL;
    SSL_CTX *ctx = NULL;
    SSL_METHOD *client_method = NULL;
    X509 *server_cert = NULL;
    char *str, *host_name, output_buf[4096], input_buf[4096], host_header[512];
    struct hostent *host_entry = NULL;
    struct sockaddr_in server_socket_address;
    struct in_addr ip;

    memset(output_buf, 0, sizeof(output_buf));
    memset(input_buf, 0, sizeof(input_buf));
    memset(host_header, 0, sizeof(host_header));
    memset(&ip, 0, sizeof(struct in_addr));

    /*========================================*/
    /* (1) initialize SSL library */
    /*========================================*/

    /*=============================================*/
    /* (2) convert server hostname into IP address */
    /*=============================================*/
    if ((fd = socket(serv->sin_family, SOCK_STREAM, 0)) == -1) 
    {
        DEBUG_SOCKET_HTTP_ERROR("Open socket error!\n - errno: %s\r\n", strerror(errno));
        if (fd)
        {
            close(fd);
        }
        return 0;
    };
    int ret = connect(fd, (struct sockaddr*)serv, sizeof(struct sockaddr_in));
    if(ret < 0)
    {
        DEBUG_SOCKET_HTTP_ERROR("Cannot connect to server port - errno: %s\r\n", strerror(errno));
    }
    DEBUG_SOCKET_HTTP_VERBOSE("(3) TCP connection open to host '%s', port:%d\n\n", domain_name, serv->sin_port); 
    /*========================================================
    */
    /* (4) initiate the SSL handshake over the TCP connection
    */

    /*========================================================*/ 
    SSLeay_add_ssl_algorithms();
    client_method = SSLv23_client_method();
    SSL_load_error_strings();
    ctx = SSL_CTX_new(client_method); 
    if(ctx == NULL)
    {
        DEBUG_SOCKET_HTTP_ERROR("Cannot create certificate ctx\r\n");
    }
    DEBUG_SOCKET_HTTP_VERBOSE("(1) SSL context initialized\n\n");
    ssl = SSL_new(ctx); /* create SSL stack endpoint*/
    if(ssl == NULL)
    {
        DEBUG_SOCKET_HTTP_ERROR("Cannot create certificate ssl\r\n");
    }

    SSL_set_fd(ssl, fd); /* attach SSL stack to socket
    */
    ret = SSL_connect(ssl); /* initiate SSL handshake */ 
    DEBUG_SOCKET_HTTP_VERBOSE("(4) SSL endpoint created & handshake completed\n\n");

    /*============================================*/
    /* (5) print out the negotiated cipher chosen */
    /*============================================*/

    printf("(5) SSL connected with cipher: %s\n\n",
    SSL_get_cipher(ssl));
    if(SSL_get_cipher(ssl) == NULL)
    {
        DEBUG_SOCKET_HTTP_VERBOSE("Cannot get cirpher \r\n");
    }
    /*========================================*/
    /* (6) print out the server's certificate */
    /*========================================*/

    server_cert = SSL_get_peer_certificate(ssl);
    
    if(server_cert == NULL)
    {
        DEBUG_SOCKET_HTTP_VERBOSE("Cannot get cirpher X509 \r\n");
    }

    DEBUG_SOCKET_HTTP_VERBOSE("(6) server's certificate was received:\n\n");

    str = X509_NAME_oneline(X509_get_subject_name(server_cert), 0, 0);
    DEBUG_SOCKET_HTTP_VERBOSE(" subject: %s\n", str);

    str = X509_NAME_oneline(X509_get_issuer_name(server_cert), 0, 0);
    DEBUG_SOCKET_HTTP_VERBOSE(" issuer: %s\n\n", str);

    /* certificate verification would happen here */

    X509_free(server_cert);

    // sprintf(host_header,"Host: %s\r\n", "www.speedtest.net");
    // strcpy(output_buf,"GET /sppedtest-config.php HTTP/1.0\r\n");
    // strcat(output_buf,host_header);
    // strcat(output_buf,"Connection: close\r\n");
    // strcat(output_buf,"\r\n");
    sprintf(output_buf,
            "GET /%s HTTP/1.0\r\n"
            "Host: %s\r\n"
            "User-Agent: status\r\n"
            "Accept: */*\r\n\r\n", "speedtest-config.php", domain_name);  

    ret = SSL_write(ssl, output_buf, strlen(output_buf));
    //shutdown (fd, 1); /* send EOF to server */

    DEBUG_SOCKET_HTTP_VERBOSE("(7) sent HTTP request over encrypted channel:\n\n%s\n", output_buf);

    /**************************************************/
    /* (8) read back HTTP response from the SSL stack */
    /**************************************************/

    ret = SSL_read(ssl, input_buf, sizeof(input_buf) - 1);
    //input_buf[ret] = '\0';
    if(ret == 0)
    {
        printf("No response\r\n");
    }
    else
    {
        printf("ret%d\r\n", ret);
    }
    DEBUG_SOCKET_HTTP_VERBOSE("(8) got back %d bytes of HTTP response:\n\n%s\n", ret, input_buf);

    /************************************************/
    /* (9) all done, so close connection & clean up */
    /************************************************/

    SSL_shutdown(ssl);
    close (fd);
    SSL_free (ssl);
    SSL_CTX_free (ctx);

    DEBUG_SOCKET_HTTP_VERBOSE("(9) all done, cleaned up and closed connection\n\n"); 
}