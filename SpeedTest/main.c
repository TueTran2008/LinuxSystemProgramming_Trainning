/**
 * @file main.c
 *
 * @brief SpeedTest Application
 *
 * This application performs network speed tests by measuring upload and download
 * speeds using specified server(s).
 */
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


/**
 * @brief Structure to store time values.
 */
static struct timeval tv;
/**
 * @brief Mutex for thread synchronization.
 */
static pthread_mutex_t app_debug_mutex = PTHREAD_MUTEX_INITIALIZER;
/**
 * @brief Default protocol for the speed test.
 */
static st_server_protocol_t protocol = SPEEDTEST_SERVER_PROCOTOL_HTTP;
/**
 * @brief Default operation for the speed test.
 */
static st_server_operation_t operation = SPEEDTEST_SERVER_OPERATION_UPLOAD;
/**
 * @brief Default number of threads for the speed test.
 */
static unsigned int number_of_thread = 10;
/**
 * @brief Flag to indicate if a custom server URL is provided.
 */
static bool perform_server_url = false;
/**
 * @brief Custom server URL provided by the user.
 */
static char *p_server_url = NULL;
/**
 * @brief Flag to indicate if only upload testing is performed.
 */
static bool  m_test_only_upload = false;
/**
 * @brief Flag to indicate if only download testing is performed.
 */
static bool  m_test_only_download = false;
/**
 * @brief Prints program usage information.
 *
 * This function prints the usage instructions for the speed test application.
 */
static void print_help(void)
{
    DEBUG_INFO("Usage (options are case sensitive):\r\n"
              "No argument: Default - pick lowest latancy among 10 nearest"
              "--help - Show this help\r\n"
              "--download_testing - Run only download testing\r\n"
              "--upload_testing - Run only upload testing\r\n"
              "--server URL - Test server URL instead of the lowest latancy\r\n"
              "--thread N - Run test with N threads\r\n"
              "HTTP - Use HTTP protocol\r\n"
              "HTTPS - Use HTTPS protocol\r\n"
    
    );
}
/**
 * @brief Gets the current timestamp in milliseconds.
 *
 * @return Current timestamp in milliseconds.
 */
static uint32_t app_get_ms(void)
{
    gettimeofday(&tv, NULL);
    unsigned long long millisecondsSinceEpoch =
        (unsigned long long)(tv.tv_sec) * 1000 +
        (unsigned long long)(tv.tv_usec) / 1000;
    return millisecondsSinceEpoch;
}
/**
 * @brief Writes data to stdout.
 *
 * @param buffer Pointer to the data buffer.
 * @param len Length of the data.
 *
 * @return Number of bytes written.
 */
static unsigned int app_puts(const void *buffer, uint32_t len)
{
    fwrite(buffer, 1, len, stdout);
    return len;
}
/**
 * @brief Acquires or releases a mutex lock.
 *
 * @param lock If true, acquires the lock. If false, releases the lock.
 * @param timeout The maximum time (in milliseconds) to wait for the lock.
 *
 * @return True if the lock operation is successful, false otherwise.
 */
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
/**
 * @brief Main function of the speed test application.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 *
 * @return 0 on successful execution, non-zero on failure.
 */
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
        else if(strcmp(argv[i], "HTTPS") == 0)
        {
            protocol = SPEEDTEST_SERVER_PROTOCOL_HTTPS;
        }
        else if(strcmp(argv[i], "HTTP") == 0)
        {
            protocol = SPEEDTEST_SERVER_PROCOTOL_HTTP;
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
            speedtest_test_domain_name(p_server_url, protocol, operation, number_of_thread);
        }
        else
        {
            operation = SPEEDTEST_SERVER_OPERATION_DOWNLOAD;
            speedtest_test_lowest_latency(protocol, operation, number_of_thread);
            operation = SPEEDTEST_SERVER_OPERATION_UPLOAD;
            speedtest_test_lowest_latency(protocol, operation, number_of_thread); 
        }
    }
}