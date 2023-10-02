#ifndef BECOME_DAEMON_H /* Prevent double inclusion */
#define BECOME_DAEMON_H
/* Bit-mask values for 'flags' argument of daemon_process_create() */
#define BD_NO_CHDIR 01 /* Don't chdir("/") */
#define BD_NO_CLOSE_FILES 02 /* Don't close all open files */
#define BD_NO_REOPEN_STD_FDS 04 /* Don't reopen stdin, stdout, and stderr to /dev/null */
#define BD_NO_UMASK0 010 /* Don't do a umask(0) */
#define BD_MAX_CLOSE 8192 /* Maximum file descriptors to close if sysconf(_SC_OPEN_MAX) is indeterminate */

/**
 * @brief Make the process becomes a daemon process
 * @param[in] flags Option when performing step Please reference to  BD_NO_CHDIR to BD_NO_UUMASK0 preprocessor
 * @return -1 if function performs fail
 *          0 if function performs successfylly
 */
int daemon_process_create(int flags);
#endif