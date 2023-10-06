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

#define FILE_DIRECTORY_PATH "/tmp"

int socket_http_get_file(struct sockaddr_in *serv, char *domain_name, char *request_url, char *filename) 
{
    int fd;
    char sbuf[256]={0}, tmp_path[128]={0};
    char rbuf[8192];
    struct timeval tv;
    fd_set fdSet;
    FILE *fp = NULL;

    memset(sbuf, 0, sizeof(sbuf));
    memset(tmp_path, 0, sizeof(tmp_path));
    memset(rbuf, 0, sizeof(sbuf));
    if ((fd = socket(serv->sin_family, SOCK_STREAM, 0)) == -1) 
    {
        perror("Open socket error!\n");
        if(fd) close(fd);
        return 0;
    }
    if(connect(fd, (struct sockaddr *)serv, sizeof(struct sockaddr)) == -1) {
        perror("Socket connect error!\n");
        if(fd) close(fd);
        return 0;
    }

    sprintf(sbuf,
            "GET /%s HTTP/1.0\r\n"
            "Host: %s\r\n"
            "User-Agent: status\r\n"
            "Accept: */*\r\n\r\n", request_url, domain_name);                                                                 

    if(send(fd, sbuf, strlen(sbuf), 0) != strlen(sbuf)) {
        perror("Can't send data to server\n");
        if(fd) close(fd);
        return 0;
    }

    sprintf(tmp_path, "%s%s", FILE_DIRECTORY_PATH, filename);
    fp = fopen(tmp_path, "w+");

    /*Perform non blocking IO read*/
    while(1) {
        char *ptr=NULL;
        memset(rbuf, 0, sizeof(rbuf));
        FD_ZERO(&fdSet);
        FD_SET(fd, &fdSet);

        tv.tv_sec = 3;
        tv.tv_usec = 0;
        int status = select(fd + 1, &fdSet, NULL, NULL, &tv);
        int i = recv(fd, rbuf, sizeof(rbuf), 0);
        if(status > 0 && FD_ISSET(fd, &fdSet)) {
            if(i < 0) {
                printf("Can't get http file!\n");
                close(fd);
                fclose(fp);
                return 0;
            } else if(i == 0) {
                break;
            } else {
                if((ptr = strstr(rbuf, "\r\n\r\n")) != NULL) {
                    ptr += 4;
                    fwrite(ptr, 1, strlen(ptr), fp);
                } else {
                    fwrite(rbuf, 1, i, fp);
                }
            }
        }
    }
    close(fd);
    fclose(fp);
    return 1;
}