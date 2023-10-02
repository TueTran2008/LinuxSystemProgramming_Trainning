#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include "time_count.h"

bool got_alarm = false;
bool got_interrupt = false;
static void signal_alarm_handle(int sig)
{
    if (sig == SIGALRM)
    {
        got_alarm = true;
    }
    else
    {
        got_interrupt = true;
    }
    

}
int main(int argc, char *argv[])
{
    struct itimerval interval_timer;
    struct sigaction sa;
    interval_timer.it_value.tv_sec = 1;
    interval_timer.it_value.tv_usec = 0;
    interval_timer.it_interval.tv_sec = 2;
    interval_timer.it_interval.tv_usec = 0;
    if (setitimer(ITIMER_REAL, &interval_timer, 0) == -1)
        printf("setitimer");
    else
    {
        time_count_begin();
    }

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = signal_alarm_handle;
    if (sigaction(SIGALRM, &sa, NULL) == -1)
        printf("Error when usesigaction");
    if(signal(SIGINT, signal_alarm_handle) == SIG_ERR)
    {
        printf("Error when regis handle for signal INT\r\n");
    }
    while (1) /*This can be done by using another thread :P*/
    {
        if(got_alarm)
        {
            got_alarm = false;
            printf("Send signal kill form %s\r\n", argv[0]);
            kill(0, SIGINT);
        }
        if (got_interrupt)
        {
            got_interrupt = false;
            printf("Receiving signal interrupt\r\n");
        }
    }
    return 0;
}