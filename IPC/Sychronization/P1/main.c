/******************************************************************************
 *                              HEADER AND DIRECTORY 
******************************************************************************/
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "file_handle.h"
/******************************************************************************
 *                              DEFINE AND TYEPDEF 
******************************************************************************/
typedef struct main
{
    /* data */
    int loops;
    int thread_id;
}thread_data_t;
/******************************************************************************
 *                              PRIVATE VARIABLES 
******************************************************************************/
static volatile int glob = 0;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
/******************************************************************************
 *                              PRIVATE FUNCTIONS 
******************************************************************************/
static void *thread_func(void *arg)
{
    thread_data_t thread_data = *((thread_data_t *) arg);
    int loc, j, s;
    char write_data[10]; /*Integer cannot overflow ten digit*/
    char file_name[64];
    s = pthread_mutex_lock(&mtx);
    for (j = 0; j < thread_data.loops; j++) {
        
        if (s != 0)
            printf("Error in pthread_mutex_lock\r\n");

        loc = glob;
        loc++;
        glob = loc;

        if (s != 0)
            printf("Error in pthread_mutex_unlock\r\n");
    }
    printf("Thread %d - glob: %d\r\n", thread_data.thread_id, glob);
    sprintf(write_data, "%d", glob);
    sprintf(file_name, "./thread%d.txt", thread_data.thread_id);
    file_handle_write(file_name, write_data);
    s = pthread_mutex_unlock(&mtx);

    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t t1, t2;
    int s;
    thread_data_t thread_data1, thread_data2;
    thread_data1.loops = 100000;
    thread_data1.thread_id = 1;
    s = pthread_create(&t1, NULL, thread_func, &thread_data1);
    if (s != 0)
    {
        printf("Error in pthread_create 1 \r\n");
    }
    thread_data2.loops = 100000;
    thread_data2.thread_id = 2;
    s = pthread_create(&t2, NULL, thread_func, &thread_data2);
    if (s != 0)
    {
        printf("Error in pthread_create 2\r\n");
    }
    s = pthread_join(t1, NULL);
    if (s != 0)
    {
        printf("Error in pthread_join 1");
    }
    s = pthread_join(t2, NULL);
    if (s != 0)
    {
        printf("Error in pthread_join 2");
    }
    printf("glob = %d\n", glob);
    exit(0);
}
