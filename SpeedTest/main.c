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


static struct timeval tv;
static pthread_mutex_t app_debug_mutex = PTHREAD_MUTEX_INITIALIZER;
static st_server_protocol_t protocol = SPEEDTEST_SERVER_PROCOTOL_HTTP;
static st_server_operation_t operation = SPEEDTEST_SERVER_OPERATION_UPLOAD;
static unsigned int number_of_thread = 3;
static bool perform_server_url = false;
static char *p_server_url = NULL;
static bool  m_test_only_upload = false;
static bool  m_test_only_download = false;

static void print_help(void)
{
    DEBUG_INFO("Usage (options are case sensitive):"
              "help - Show this help"
              "--download_testing - Run only download testing"
              "--upload_testing - Run only upload testing"
              "--server URL - use server URL, don't read config"
    
    );
}

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
    if (lock)
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
    if (argc < 2)
    {
        int ret = 0;
        operation = SPEEDTEST_SERVER_OPERATION_UPLOAD;
        ret = speedtest_test_lowest_latency(protocol, operation, number_of_thread);
        operation = SPEEDTEST_SERVER_OPERATION_DOWNLOAD;
        ret = speedtest_test_lowest_latency(protocol, operation, number_of_thread);
        return 0;
    }
    for (unsigned int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i] ,"--help") == 0)
        {
            print_help();
            return 0;
        }
        if (strcmp(argv[i], "--server") == 0)
        {
            if(argv[i + 1] != NULL)
            {
                p_server_url = argv[i + 1];
                perform_server_url = true;
            }
        }
        else if ((strcmp(argv[i], "--thread") == 0))
        {
            if(argv[i + 1] != NULL)
            {
                number_of_thread = atoi(argv[i + 1]);
            }
        }   
        else if (strcmp(argv[i], "--upload_testing") == 0)
        {
            operation = SPEEDTEST_SERVER_OPERATION_UPLOAD;
            if (m_test_only_download == true)
            {
                DEBUG_ERROR("Already test only download\r\n");
                return 0;
            }
            else
            {
                m_test_only_upload = true;
            }
        }
        else if (strcmp(argv[i], "--download_testing") == 0)
        {
            operation = SPEEDTEST_SERVER_OPERATION_DOWNLOAD;
            if (m_test_only_upload == true)
            {
                DEBUG_ERROR("Already test only download\r\n");
                return 0;
            }
            else
            {
                m_test_only_download = true;
            }
        }
    }
    if (m_test_only_download)
    {
        operation = SPEEDTEST_SERVER_OPERATION_DOWNLOAD;
        if (perform_server_url)
        {
            speedtest_test_domain_name(p_server_url, protocol, operation, number_of_thread);
        }
        else
        {
            speedtest_test_lowest_latency(protocol, operation, number_of_thread);
        }
    }
    else if (m_test_only_upload)
    {
        operation = SPEEDTEST_SERVER_OPERATION_DOWNLOAD;
        if (perform_server_url)
        {
            speedtest_test_domain_name(p_server_url, protocol, operation, number_of_thread);
        }
        else
        {
            speedtest_test_lowest_latency(protocol, operation, number_of_thread);
        }
    }
    else 
    {
        if (perform_server_url)
        {
            operation = SPEEDTEST_SERVER_OPERATION_DOWNLOAD;
            speedtest_test_domain_name(p_server_url, protocol, operation, number_of_thread);
            operation = SPEEDTEST_SERVER_OPERATION_UPLOAD;
            speedtest_test_lowest_latency(protocol, operation, number_of_thread);
        }
        else
        {
            operation = SPEEDTEST_SERVER_OPERATION_DOWNLOAD;
            speedtest_test_domain_name(p_server_url, protocol, operation, number_of_thread);
            operation = SPEEDTEST_SERVER_OPERATION_UPLOAD;
            speedtest_test_lowest_latency(protocol, operation, number_of_thread); 
        }
    }
}