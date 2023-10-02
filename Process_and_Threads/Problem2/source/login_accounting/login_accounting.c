
/******************************************************************************
 *                              HEADER AND DIRECTORY 
******************************************************************************/
#define _GNU_SOURCE
#include <time.h>
#include <utmpx.h>
#include <paths.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

/******************************************************************************
 *                              PRIVTE VARIABLE
******************************************************************************/
static const char* LOGIN_ACCOUNTING_DAEMON_STRING = "TUE_LOGIN";
/******************************************************************************
 *                              GLOBAL FUNCTION
******************************************************************************/
void login_accounting_log_login_message(void)
{
    struct utmpx *ut;
    openlog(LOGIN_ACCOUNTING_DAEMON_STRING, LOG_DAEMON | LOG_PID | LOG_CONS | LOG_NOWAIT, LOG_LOCAL3);
    setutxent();
    syslog(LOG_USER | LOG_INFO, "user type PID line id host date/time\n");
    while ((ut = getutxent()) != NULL) 
    { /* Sequential scan to EOF */
        syslog(LOG_USER | LOG_INFO, "%-8s ", ut->ut_user);
        syslog(LOG_USER | LOG_INFO, "%-9.9s ",
        (ut->ut_type == EMPTY) ? "EMPTY" :
        (ut->ut_type == RUN_LVL) ? "RUN_LVL" :
        (ut->ut_type == BOOT_TIME) ? "BOOT_TIME" :
        (ut->ut_type == NEW_TIME) ? "NEW_TIME" :
        (ut->ut_type == OLD_TIME) ? "OLD_TIME" :
        (ut->ut_type == INIT_PROCESS) ? "INIT_PR" :
        (ut->ut_type == LOGIN_PROCESS) ? "LOGIN_PR" :
        (ut->ut_type == USER_PROCESS) ? "USER_PR" :
        (ut->ut_type == DEAD_PROCESS) ? "DEAD_PR" : "???");
        syslog(LOG_USER | LOG_INFO, "%5ld %-6.6s %-3.5s %-9.9s ", (long) ut->ut_pid,
        ut->ut_line, ut->ut_id, ut->ut_host);
        syslog(LOG_USER | LOG_INFO, "%s", ctime((time_t *) &(ut->ut_tv.tv_sec)));
    }
    endutxent();
    closelog();
}