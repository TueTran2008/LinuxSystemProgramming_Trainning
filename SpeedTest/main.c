#include "socket_ip.h"
#include "socket_http.h"
#include "speedtest.h"
#include "speedtest_utilities.h"
#include "app_debug.h"
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>





static uint32_t app_get_ms(void)
{
    gettimeofday(&tv, NULL);
    unsigned long long millisecondsSinceEpoch =
        (unsigned long long)(tv.tv_sec) * 1000 +
        (unsigned long long)(tv.tv_usec) / 1000;
    return millisecondsSinceEpoch;
}

static unsigned int app_puts(const void *buffer, uint32_t len)
{
    fwrite(buffer, 1, len, stdout);
    return len;
}
static bool app_lock(bool lock, uint32_t timeout)
{
    (void)timeout; /*Lock forever this case*/
    if(lock)
    {
        return pthread_mutex_lock(&app_debug_mutex);
    }
    else
    {
        return pthread_mutex_unlock(&app_debug_mutex);
    }
}

int main(int argc, char *argv[])
{
    app_debug_register_callback_print(app_puts); /*Install callback print function*/
    app_debug_init(app_get_ms, app_lock);
    if()
    {
        
    }
}