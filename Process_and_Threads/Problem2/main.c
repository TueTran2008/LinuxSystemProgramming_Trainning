#include <stdio.h>
#include <unistd.h>
#include "daemon.h"
#include "login_accounting.h"
int main()
{
    daemon_process_create(0);
    login_accounting_log_login_message();
}