/*
SYNOPSIS

    Solution for excercise 40-03.

REQUIREMENT

    Read the manual pages for login(3), logout(3), and logwtmp(3).
    Implement these functions.

SOLUTION

    Implement login(3)

        - overwrite ut->ut_type by USER_PROCESS
        - overwrite ut->ut_pid by process id of calling process
        - overwrite ut->ut_line with terminal device of stdin. in case no file
          description connected with terminal, fill by '???' and do not write
          to utmp
        - write ut structure to utmp by pututxline()
        - write ut structure to wtmp by updwtmpx()

    Implement logout(3)

        - create utmpx struct called ut
        - set ut.ut_line by ut_line param
        - set ut.ut_id from ut_line param
        - set ut.ut_user by zero
        - set ut.ut_tv.tv_sec by current time
        - remove login record from utmp by pututxline()
        - fill more field as ut_time, ut_pid, ut_session to ut
        - append logout record to wtmp by updwtmpx()

    Implement logwtmp(3)

        - create utmpx struct called ut
        - set ut.ut_tv.tv_sec by current time
        - set ut.ut_pid by id of calling process
        - set ut.ut_line by line param
        - set ut.ut_name by name param
        - set ut.ut_host by host param
        - write record to wtmp by updwtmpx()

    Implement main()

        - verify arguments
        - switch to call do_login(), do_logout(), do_logwtmp()

USAGE

    $ ./dest/bin/40-03 login
    $ last | grep someone
    someone  pts/0        dev          Wed Jun 21 22:34    gone - no logout

    $ ./dest/bin/40-03 logout
    someone  pts/0        dev          Wed Jun 21 22:34 - 22:34  (00:00)

    $ ./dest/bin/40-03 logwtmp
    $ last | grep someone
    someone  pts/0        dev          Wed Jun 21 22:35    gone - no logout

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#define _GNU_SOURCE

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <utmpx.h>
#include <libgen.h>
#include <paths.h>
#include <time.h>
#include <limits.h>

#include <tlpiexer/error.h>

#define RET_SUCCESS 0
#define RET_FAILURE -1

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : set of function try to perform implemented functions
int do_login(void);
int do_logout(void);
int do_logwtmp(void);

// spec     : write login information to utmp, wtmp
// arg      :
//  - ut: data structure contain login information
// ret      :
//  - 0 on success
//  - -1 on error
void login(const struct utmpx *ut);

// spec     : earase login information from utmp
// arg      :
//  - ut_line: terminal identity. terminal path follow format '/dev/<id>',
//    ut_line is terminal path remove by '/dev/'.
//    sample terminal path is '/dev/pts/2', ut_line will be 'pts/2'
// ret      :
//  - 0 on success
//  - -1 on fails
int logout(const char *ut_line); 

// spec     : append an loging record to wtmp
// arg      :
//  - line: terminal id.
//    sample is path to terminal is '/dev/pts/2', line will be 'pts/2'
//  - name: ?
//  - host: name of host
// ret      : none
void logwtmp(const char *line, const char *name, const char *host);

int main(int argc, char *argv[])
{
    // show help
    if (argc == 2 && strcmp(argv[1], "-h") == 0) {
        showhelp();
        return EXIT_SUCCESS;
    }

    // verify arguemnts
    if (argc != 2) {
        showhelp();
        return EXIT_FAILURE;
    }

    // swith to perform one in three functions
    // login(3), logout(3), logwtmp(3)
    if (strcmp(argv[1], "login") == 0) 
        return do_login();
    else if (strcmp(argv[1], "logout") == 0)
        return do_logout();
    else if (strcmp(argv[1], "logwtmp") == 0)
        return do_logwtmp();
    else {
        showhelp();
        return EXIT_FAILURE;
    }
}

void showhelp()
{
    // variables
    char exename[] = "40-3";

    printf("\nUSAGE:\n\n");
    printf("\t%s login\n", exename);
    printf("\t%s logout\n", exename);
    printf("\t%s logwtmp\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("ARGUMENTS:\n\n");
    printf("\tlogin: perform login(3)\n");
    printf("\tlogout: perform logout(3)\n");
    printf("\tlogwmpt: perform logwtmp(3)\n");
}

int do_login(void)
{
    // variables
    struct utmpx ut;

    // init
    strcpy(ut.ut_user, "someone");
    if (gethostname(ut.ut_host, sizeof(ut.ut_host)) == -1)
        return RET_FAILURE;
    ut.ut_session = getsid(0);

    // perform login(3)
    login(&ut);

    // success
    return RET_SUCCESS;
}

int do_logout(void)
{
    // variables
    char *ttypath, *ut_line;

    // retrieve tty id
    ttypath = ttyname(STDIN_FILENO);
    if (ttypath == NULL || strlen(ttypath) < 8)
        return RET_FAILURE;
    ut_line = ttypath + 5;

    // perform logout(3)
    return logout(ut_line);
}

int do_logwtmp(void)
{
    // variables
    char *ttypath, *ut_line;
    char hostname[HOST_NAME_MAX];

    // retrieve tty id
    ttypath = ttyname(STDIN_FILENO);
    if (ttypath == NULL || strlen(ttypath) < 8)
        return RET_FAILURE;
    ut_line = ttypath + 5;

    // retrieve host name
    if (gethostname(hostname, HOST_NAME_MAX) == -1)
        return RET_FAILURE;

    // perform logwtmp(3)
    logwtmp(ut_line, "someone", hostname);
    return 0;
}

void login(const struct utmpx *ut)
{
    // variables
    char *ttypath;
    struct utmpx nut;

    // copy information
    memcpy(&nut, ut, sizeof(nut));

    // overwrite value
    nut.ut_type = USER_PROCESS;
    nut.ut_pid = getpid();
    ttypath = ttyname(STDIN_FILENO);
    if (ttypath == NULL || strlen(ttypath) < 8) {
        strcpy(nut.ut_line, "???");
    } else {
        strcpy(nut.ut_line, ttypath + 5);
        strcpy(nut.ut_id, basename(ttypath));
    }
    time((time_t *) &nut.ut_tv.tv_sec);
    
    // write to utmp
    // not care about error
    if (strcmp(nut.ut_line, "???") != 0)
        pututxline(&nut);

    // write to wtmp
    // not care about error
    updwtmpx(_PATH_WTMP, &nut);
}

int logout(const char *ut_line)
{
    // variables
    struct utmpx ut;
    char eut_line[32];

    // init
    strcpy(eut_line, ut_line);
    memset(&ut, 0, sizeof(ut));

    // prepare utmp structure to remove from utmp, write to wtmp
    ut.ut_type = DEAD_PROCESS;
    ut.ut_tv.tv_sec = 0;
    strcpy(ut.ut_line, ut_line);
    strcpy(ut.ut_id, basename(eut_line));
    if (time((time_t *) &ut.ut_tv.tv_sec) == -1)
        return RET_FAILURE;

    // remove utmp structure from utmp
    setutxent();
    if (pututxline(&ut) == NULL)
        return RET_FAILURE;
    
    // fill some information more
    // append utmp structure to wtmp
    ut.ut_pid = getpid();
    if (gethostname(ut.ut_host, sizeof(ut.ut_host)) == -1)
        return RET_FAILURE;
    ut.ut_session = getsid(0);
    updwtmpx(_PATH_WTMP, &ut);

    // success
    return RET_SUCCESS;
}

void logwtmp(const char *line, const char *name, const char *host)
{
    // variables
    struct utmpx ut;

    // prepare login record
    if (time((time_t *) &ut.ut_tv.tv_sec) == -1)
        ut.ut_tv.tv_sec = 0;
    ut.ut_pid = getpid();
    strncpy(ut.ut_line, line, sizeof(ut.ut_line));
    strncpy(ut.ut_user, name, sizeof(ut.ut_user));
    strncpy(ut.ut_host, host, sizeof(ut.ut_host));

    // write login record to wtmp
    updwtmpx(_PATH_WTMP, &ut);
}
