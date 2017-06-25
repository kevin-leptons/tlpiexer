/*
SYNOPSIS

    Solution for exercise 40-02.

REQUIREMENT

    Modify the program in listing 40-3 (utmpx_login.c) so that it updates
    the lastlog file in addition to the utmp and wtmp files.

SOLUTION

    Implement wlastlog()

        - open log file by open()
        - seek to user offset by lseek()
        - write lastlog structure to file by write()
        - close file by close()

    Implement main()

        - copy source code in tlpi/loginacct/utmpx_login.c
        - append code statement to overwrite lastlog structure to file locate 
          by _PATH_LASTLOG. use wlastlog() has implemented above

USAGE

    $ ./dest/bin/40-02 kevin
    creating login entries in utmp, wtmp, lastlog
    using pid 11815, line pts/1, id 1
    creating logout entries in utmp and wtmp

    $ lastlog
    ...
    kevin            pts/1    dev              Wed Jun 21 20:58:28 +0700 2017

AUTHORS

    Michael Kerrisk
    Kevin Leptons <kevin.leptons@gmail.com>
*/

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <utmpx.h>
#include <paths.h>
#include <stdarg.h>
#include <lastlog.h>
#include <fcntl.h>

#include <tlpiexer/error.h>

#define RET_SUCCESS 0
#define RET_FAILURE -1

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : show error message then exit process with EXIT_FAILURE
// arg      :
//  - format: format string and params same as printf
// ret      : none
void fatal(char *format, ...);

// spec     : write lastlog to file
// arg      :
//  - file: path to log file, if specific as NULL, _PATH_LASTLOG will be used
//  - uid: user identify to write to lastlog file
//  - log: information about loging
// ret      :
//  - 0 on success
//  - -1 on error
int wlastlog(char *file, uid_t uid, struct lastlog *log);

int main(int argc, char *argv[])
{
    // variable
    struct utmpx ut;
    char *devname;
    struct lastlog llog;
    uid_t uid;

    // show help
    if (argc == 2 && strcmp(argv[1], "-h") == 0) {
        showhelp();
        return EXIT_SUCCESS;
    }

    // incorrect arguments
    if (argc < 2) {
        showhelp();
        return EXIT_FAILURE;
    }

    // initialize login record for utmp and  wtmp files
    memset(&ut, 0, sizeof(struct utmpx));
    ut.ut_type = USER_PROCESS;
    strncpy(ut.ut_user, argv[1], sizeof(ut.ut_user));
    if (time((time_t*) &ut.ut_tv.tv_sec) == -1)
        exit_err("time");
    ut.ut_pid = getpid();

    // set ut_line and ut_id based on the terminal associated with
    // 'stdin'. this code assumes terminals named '/dev/[pt]t[sy]*'.
    // the '/dev/' dirname is 5 characters; the '[pt]t[sy]' filename
    // prefix is 3 characters (making 8 characters in all)
    if ((devname = ttyname(STDIN_FILENO)) == NULL)
        exit_err("ttyname");
    if (strlen(devname) <= 8)
        fatal("terminal name is too short: %s", devname);
    strncpy(ut.ut_line, devname + 5, sizeof(ut.ut_line));
    strncpy(ut.ut_id, devname + 9, sizeof(ut.ut_id));

    // init laslog structure
    memset(&llog, 0, sizeof(llog));
    uid = getuid();
    llog.ll_time = ut.ut_tv.tv_sec;
    strcpy(llog.ll_line, ut.ut_line);
    if (gethostname(llog.ll_host, sizeof(llog.ll_host)) == -1)
        exit_err("gethostname");

    // show information
    printf("creating login entries in utmp, wtmp, lastlog\n");
    printf("using pid %ld, line %.*s, id %.*s\n",
            (long) ut.ut_pid, (int) sizeof(ut.ut_line), ut.ut_line,
            (int) sizeof(ut.ut_id), ut.ut_id);

    // overwrite login record to utmp
    // append login record to wtmp
    // overwrite login to lastlog
    setutxent();
    if (pututxline(&ut) == NULL)
        exit_err("pututxline");
    updwtmpx(_PATH_WTMP, &ut);
    wlastlog(NULL, uid, &llog);

    // now do a 'logout'; use values from previously initialized 'ut',
    // except for changes below
    ut.ut_type = DEAD_PROCESS;
    time((time_t*) &ut.ut_tv.tv_sec);
    memset(&ut.ut_user, 0, sizeof(ut.ut_user));


    // show information
    // overwrite logout recored to utmp
    // append logout recored to wtmp
    printf("creating logout entries in utmp and wtmp\n");
    setutxent();
    if (pututxline(&ut) == NULL)
        exit_err("pututxline");
    updwtmpx(_PATH_WTMP, &ut);
    endutxent();

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "40-2";

    printf("\nUSAGE:\n\n");
    printf("\t%s username\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("ARGUMENTS:\n\n");
    printf("\tusername: username to try loggin accounting\n\n");
}

void fatal(char *format, ...)
{
    // variables
    va_list va_args;

    va_start(va_args, format);
    printf(format, va_args);
    va_end(va_args);
}

int wlastlog(char *file, uid_t uid, struct lastlog *log)
{
    // variables
    int fd;

    // init
    if (file == NULL)
        file = _PATH_LASTLOG;

    // open lastlog file
    if ((fd = open(file, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR)) == -1)
        return RET_FAILURE;

    // seek to user location
    if (lseek(fd, uid * sizeof(struct lastlog), SEEK_SET) == -1)
        return RET_FAILURE;

    // write lastlog structure
    if (write(fd, log, sizeof(struct lastlog)) ==- 1)
        return RET_FAILURE;

    // close lastlog file
    if (close(fd) == -1)
        return RET_FAILURE;

    // success
    return RET_SUCCESS;
}
