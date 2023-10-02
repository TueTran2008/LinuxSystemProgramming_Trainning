#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

static bool is_sig_interrupt = false;
/*!
* @brief Handler for various signal when assign to signal systemcall
* @param[in] sig Signal type to be handle
*/
static void signal_handler(int sig)
{
    /**/
    if (sig == SIGINT)
    {
        is_sig_interrupt = true;
    }
    return;
}
int main(int argc, char *argv[])
{
    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        printf("Error when using signal\r\n");
        return 0;
    }
    while (1)
    {
        if (is_sig_interrupt)
        {
            is_sig_interrupt = false;
            printf("Continue running with signal interrupt\r\n");
        }
    }
}