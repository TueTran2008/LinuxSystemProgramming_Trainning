/******************************************************************************
 *                              HEADER AND DIRECTORY 
******************************************************************************/
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include "utilities.h"
#include "file_handle.h"
/******************************************************************************
 *                              DEFINE AND TYPEDEF 
******************************************************************************/
#define FILE_COMMOM "process_sync.txt"
#define START_CHAR '+'
#define STOP_CHAR '*'
#define SEPERATE_CHAR ':'
#define SYNC_SIG SIGUSR1                /* Synchronization signal */

/******************************************************************************
 *                              PRIVATE VARIABLE
******************************************************************************/
int my_counter = 0;

/*!
 * @brief           /* Signal handler - does nothing but return 
 * @param[in]       sig Signal ID
 */
/******************************************************************************
 *                              PRIVATE FUNCTION
******************************************************************************/
static void handler(int sig)
{
    return;
}
/*!
 * @brief   Read the file to get the current counter between two process
 */
static int process_file_operation_read(void)
{
    char read_data[50];    /*Buffer read from file*/
    char filter_data[50];   /*Buffer store KEY and VALUE data*/
    unsigned int data_len = 0;      /*Index of read buffer*/
    char count_buffer[20];
    if(file_handle_check_file_exist(FILE_COMMOM))
    {
        int file_size = file_handle_get_file_size(FILE_COMMOM);
        while(1)
        {
            file_handle_read(FILE_COMMOM, read_data, data_len, data_len + sizeof(read_data));   /*Read data from file to get the lastest value*/
            int start_index = utilities_find_index_of_char(START_CHAR, read_data);  /*Find start byte*/
            int stop_index = utilities_find_index_of_char(STOP_CHAR, read_data);    /*Find stop byte*/
            if(stop_index == -1 || start_index == -1)
            {
                break;
            }
            int ret = utilities_copy_parameter(read_data, filter_data, START_CHAR, STOP_CHAR);
            if(ret == 0)    /*Cannot find next data -> use the current data*/
            {
                break;
            }
            data_len = data_len + stop_index + 2;   /**/
        }
        int seperate_index = utilities_find_index_of_char(SEPERATE_CHAR, filter_data); /*Copy from zero index to ':' Character*/
        memcpy(count_buffer, filter_data, seperate_index);  
        strcpy(count_buffer, filter_data + seperate_index + 1);
        int count = atoi(count_buffer);
        return count;
    }
    else
    {
        return 0;
    }
}
/*!
 * @brief   Increate counter and write to the common file
 */
static void process_file_operation_write(int *counter)
{
    char write_data[100]; /*Buffer write to file*/
    for(int i = 0; i < 100000; i++)
    {
        (*counter) = (*counter) + 1;
    }
    sprintf(write_data, "%c%d%c%d%c\n", START_CHAR, getpid(), SEPERATE_CHAR, (*counter), STOP_CHAR);
    file_handle_write_append(FILE_COMMOM, write_data);
}

int main(int argc, char *argv[])
{
    pid_t childPid;
    sigset_t blockMask, origMask, emptyMask;
    struct sigaction sa;

    setbuf(stdout, NULL);               /* Disable buffering of stdout */

    sigemptyset(&blockMask);
    sigaddset(&blockMask, SYNC_SIG);    /* Block signal */
    if (sigprocmask(SIG_BLOCK, &blockMask, &origMask) == -1)
        printf("sigprocmask");

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    if (sigaction(SYNC_SIG, &sa, NULL) == -1)
        printf("sigaction");

    switch (childPid = fork()) 
    {
        case -1:
        {
            printf("fork");
            exit(1);
        }
        case 0: /* Child */
        {
            /* Child does some required action here... */
            while(1)
            {
                printf("[%s %ld] Child started - doing some work\n", utilities_get_time("%T"), (long) getpid());
                sleep(1);
                my_counter = process_file_operation_read();
                process_file_operation_write(&my_counter);
                /* And then signals parent that it's done */

                printf("[%s %ld] Child about to signal parent\n",
                        utilities_get_time("%T"), (long) getpid());
                if (kill(getppid(), SYNC_SIG) == -1)
                    printf("kill");
                /* Now child can do other things... */
                printf("Child waits for parent signal\r\n");
                sigemptyset(&emptyMask);
                if (sigsuspend(&emptyMask) == -1 && errno != EINTR)
                    printf("sigsuspend");
            }
            /*Never reach this*/
            exit(0);
        }
        default: /* Parent */
        {
            /* Parent may do some work here, and then waits for child to
            complete the required action */
            while(1)
            {
                printf("[%s %ld] Parent about to wait for signal\n",
                        utilities_get_time("%T"), (long) getpid());
                sigemptyset(&emptyMask);
                if (sigsuspend(&emptyMask) == -1 && errno != EINTR)
                    printf("sigsuspend");
                printf("[%s %ld] Parent got signal\n", utilities_get_time("%T"), (long) getpid());

                my_counter = process_file_operation_read();
                process_file_operation_write(&my_counter);
                sleep(1);
                if (kill(childPid, SYNC_SIG) == -1)
                    printf("kill");
            }
            exit(EXIT_SUCCESS);
        }
    }
}
