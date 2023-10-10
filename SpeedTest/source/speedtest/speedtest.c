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
 * @brief Gets system uptime.
 *
 * @return Uptime in seconds, -1 on failure.
 */
static float get_uptime(void) 
{
    FILE* fp;
    float uptime, idle_time;

    if ((fp = fopen("/proc/uptime", "r"))!=NULL) 
    {
        fscanf (fp, "%f %f\n", &uptime, &idle_time);
        fclose (fp);
        return uptime;
    }
    return -1;
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
    if ((fd = socket(t_arg->servinfo.ai_family, SOCK_STREAM, 0)) == -1) 
    {    
        DEBUG_SPEEDTEST_ERROR("Open socket error! - ernno: %d\r\n\n");
        sleep(5);
        goto err;
    }
    if (connect(fd, (struct sockaddr *)t_arg->servinfo.ai_addr, sizeof(struct sockaddr)) == -1) {
        DEBUG_SPEEDTEST_ERROR("Socket connect error!\n");
        goto err;
    }
    sprintf(sbuf,
            "GET /%s HTTP/1.0\r\n"
            "Host: %s\r\n"
            "User-Agent: status\r\n"
            "Accept: */*\r\n\r\n", t_arg->request_url, t_arg->domain_name);

    if(send(fd, sbuf, strlen(sbuf), 0) != strlen(sbuf)) 
    {
        DEBUG_SPEEDTEST_ERROR("Can't send data to server\n");
        goto err;
    }
    DEBUG_SPEEDTEST_VERBOSE("%s: HTTP Get message: %s\r\n", __FUNCTION__, sbuf);
    while(1) 
    {
        /*None Blocking API*/
        FD_ZERO(&fdSet);
        FD_SET(fd, &fdSet);

        tv.tv_sec = 3;
        tv.tv_usec = 0;
        int status = select(fd + 1, &fdSet, NULL, NULL, &tv);                                                         

        int recv_byte = recv(fd, rbuf, sizeof(rbuf), 0);
        if(status > 0 && FD_ISSET(fd, &fdSet)) {
            if(recv_byte < 0) {
                printf("Can't receive data!\n");
                break;
            } else if(recv_byte == 0){
                break;
            } else {
                pthread_mutex_lock(&pthread_mutex);
                total_dl_size += recv_byte;
                pthread_mutex_unlock(&pthread_mutex);
            }

            if(thread_all_stop)
                break;
        }   
    }
err: 
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

    st_thread_data_t *t_arg = (st_thread_data_t*)arg;
    i = t_arg->thread_index;

    memset(data, 0, sizeof(char) * UL_BUFFER_SIZE);

    if((fd = socket(t_arg->servinfo.ai_family, SOCK_STREAM, 0)) == -1) {                                                  
        DEBUG_SPEEDTEST_ERROR("Open socket error!\n");
        goto err;
    }

    if (connect(fd, (struct sockaddr *)t_arg->servinfo.ai_addr, sizeof(struct sockaddr)) == -1){
        DEBUG_SPEEDTEST_ERROR("Socket connect error!\n");
        goto err;
    }

    sprintf(sbuf,
            "POST /%s HTTP/1.0\r\n"
            "Content-type: application/x-www-form-urlencoded\r\n"
            "Host: %s\r\n"
            "Content-Length: %ld\r\n\r\n", t_arg->request_url, t_arg->domain_name, sizeof(data) * UL_BUFFER_TIMES);

    if((size = send(fd, sbuf, strlen(sbuf), 0)) != strlen(sbuf)) 
    {
        printf("Can't send header to server\n");
        goto err;
    }

    pthread_mutex_lock(&pthread_mutex);
    total_ul_size += size;
    pthread_mutex_unlock(&pthread_mutex);

    for (j = 0; j < UL_BUFFER_TIMES; j++) 
    {
        if ((size = send(fd, data, sizeof(data), 0)) != sizeof(data)) 
        {
            printf("Can't send data to server\n");
            goto err;
        }
        pthread_mutex_lock(&pthread_mutex);
        total_ul_size += size;
        pthread_mutex_unlock(&pthread_mutex);
        if(thread_all_stop)
            goto err;
    }

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
err: 
    if (fd) 
        close(fd);
    t_arg->running = 0;
    return NULL;
}
/**
 * @brief Calculate update speed thread function.
 *
 * @param arg Unused argument.
 * @return NULL.
 */
void *calculate_ul_speed_thread() 
{
    double ul_speed = 0.0, duration = 0;
    while(1) 
    {
        stop_ul_time = get_uptime();
        duration = stop_ul_time-start_ul_time;
        ul_speed = (double)total_ul_size /1000 /1000 /duration *8;
        if(duration>0) {
            printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bUpload speed: %0.2lf Mbps", ul_speed);
            fflush(stdout);
        }
        usleep(500000);

        if (thread_all_stop) 
        {
            stop_ul_time = get_uptime();
            duration = stop_ul_time-start_ul_time;
            //ul_speed = (double)total_ul_size/1024/1024/duration*8;
            ul_speed = (double)total_ul_size/1000/1000/duration*8;
            if (duration) 
            {
                printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bUpload speed: %0.2lf Mbps", ul_speed);
                fflush(stdout);
            }
            break;
        }
    }
    return NULL;
}
/**
 * @brief Calculate download speed thread function.
 *
 * @param arg Unused argument.
 * @return NULL.
 */
static void *calculate_dl_speed_thread(void *arg)
{
    (void)(arg);
    double dl_speed = 0.0, duration = 0;
    while(1) 
    {
        stop_dl_time = get_uptime();
        //printf("Stop download time: %f\r\n", stop_dl_time);
        duration = stop_dl_time - start_dl_time;
        dl_speed = (double)total_dl_size / 1000 / 1000 / duration * 8;
        if(duration > 0) 
        {
            printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bDownload speed: %0.2lf Mbps", dl_speed);
            fflush(stdout);
        }
        usleep(500000);
        if(thread_all_stop) 
        {
            stop_dl_time = get_uptime();
            duration = stop_dl_time-start_dl_time;
            //dl_speed = (double)total_dl_size/1024/1024/duration*8;
            dl_speed = (double)total_dl_size/1000/1000/duration*8;
            if(duration>0) 
            {
                printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bDownload speed: %0.2lf Mbps", dl_speed);
                fflush(stdout);
            }   
            break;
        }
    }
    return NULL;
}
/**
 * @brief Initiates speed test download.
 *
 * @param nearest_server Pointer to the nearest server data.
 * @param number_of_thread Number of threads to use for download.
 * @return 1 on success, -1 on failure.
 */
int speedtest_download(server_data_t *nearest_server, unsigned int number_of_thread) 
{
    const char download_filename[64] = "random3500x3500.jpg";  //23MB
    char url[128]= {0}, request_url[128] = {0}, dummy[128] = {0}, buf[128];
    char *ptr = NULL;
    int i;
    //pthread_t *download_thread = (pthread_t *)malloc(number_of_thread * sizeof(pthread_t));
    st_thread_data_t *download_thread_data = (st_thread_data_t*)malloc(number_of_thread * sizeof(st_thread_data_t));
    memset(download_thread_data, 0, sizeof(st_thread_data_t) * number_of_thread);

    pthread_t calculate_thread;
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
    pthread_create(&calculate_thread, NULL, calculate_dl_speed_thread, NULL);
    /*Get the URL of the download site*/
    /*speedtesthni2.viettel.vn:8080
        http://speedtesthni2.viettel.vn:8080/speedtest/upload.php
        request_url = speedtest/upload.php
        dummy = speedtesthni2.viettel.vn:8080
    */
    sscanf(nearest_server->url, "http://%[^/]/%s", dummy, request_url);
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
    DEBUG_SPEEDTEST_INFO("%s: request_url:%s\r\n", __FUNCTION__, request_url);
    start_dl_time = get_uptime();
    while(1) 
    {
        for(i = 0; i < number_of_thread; i++) 
        {
            memcpy(&download_thread_data[i].servinfo, &nearest_server->servinfo, sizeof(download_thread_data[i].servinfo));
            memcpy(download_thread_data[i].domain_name, nearest_server->domain_name, sizeof(nearest_server->domain_name));
            memcpy(download_thread_data[i].request_url, request_url, sizeof(request_url));
            
            if(download_thread_data[i].running == 0) 
            {
                download_thread_data[i].thread_index = i;
                download_thread_data[i].running = 1;
                pthread_create(&download_thread_data[i].tid, NULL, download_thread, &download_thread_data[i]);
            }
        }
        if (thread_all_stop)
        {
            break;
        }
    }
    free(download_thread_data);
    pthread_detach(calculate_thread);
    return 1;
}
/**
 * @brief Initiates speed test upload.
 *
 * @param nearest_server Pointer to the nearest server data.
 * @param number_of_thread Number of threads to use for upload.
 * @return 1 on success, -1 on failure.
 */
int speedtest_upload(server_data_t *nearest_server, unsigned int number_of_thread) 
{
    int i;
    char dummy[128]={0}, request_url[128]={0};
    sscanf(nearest_server->url, "http://%[^/]/%s", dummy, request_url);
    start_ul_time = get_uptime();
    pthread_t calculate_thread;
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
    st_thread_data_t *up_thread_data = (st_thread_data_t*)malloc(number_of_thread * sizeof(st_thread_data_t));
    if(up_thread_data == NULL)
    {
        DEBUG_SPEEDTEST_ERROR("Cannot allocate memory of upload thread\r\n");
        return -1;
    }
    memset(up_thread_data, 0, sizeof(st_thread_data_t) * number_of_thread);
    pthread_create(&calculate_thread, NULL, calculate_ul_speed_thread, NULL);
    while(1) 
    {
        for(i = 0; i < number_of_thread; i++) 
        {
            memcpy(&up_thread_data[i].servinfo, &nearest_server->servinfo, sizeof(up_thread_data[i].servinfo));
            memcpy(up_thread_data[i].domain_name, nearest_server->domain_name, sizeof(nearest_server->domain_name));
            memcpy(up_thread_data[i].request_url, request_url, sizeof(request_url));
            if(up_thread_data[i].running == 0) 
            {
                up_thread_data[i].thread_index = i;
                up_thread_data[i].running = 1;
                pthread_create(&up_thread_data[i].tid, NULL, upload_thread, &up_thread_data[i]);
            }
        }
        if(thread_all_stop)
            break;
    }
    free(up_thread_data);
    pthread_detach(calculate_thread);
    return 1;
}
/**
 * @brief Handles SIGALRM signal to stop all threads.
 *
 * @param signo Signal number.
 */
void speedtest_stop_all_thread(int signo) 
{
    if (signo == SIGALRM) 
    {
        thread_all_stop=1;
    }
    return;
}