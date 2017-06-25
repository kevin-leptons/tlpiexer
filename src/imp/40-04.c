/*
SYNOPSIS

    Solution for exercise 40-04.

REQUIREMENT

    Implement simple version of who(1).

SOLUTION :

    - parse arguments
    - sure that all of utmp function use _PATH_UTMP by utmpname()
    - retrieve all of record in utmp, filter ut_type by USER_PROCESS,
      and show information

USAGE

    $ ./dest/bin/40-04
    kevin            pts/1                        39800447-11-03 16:54:13
    someone          pts/1                        2017-06-21 22:06:02  

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <utmp.h>
#include <utmpx.h>
#include <time.h>
#include <paths.h>
#include <string.h>

#include <tlpiexer/error.h>

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

int main(int argc, char *argv[])
{
    // variables
    struct utmpx *ut;
    char timestr[32];
    struct tm *t;

    // show help
    if (argc == 2 && strcmp(argv[1], "-h") == 0) {
        showhelp();
        return EXIT_SUCCESS;
    }

    // verify arguments
    if (argc > 1) {
        showhelp();
        return EXIT_FAILURE;
    }

    // retrieve all of logged on record
    utmpname(_PATH_UTMP);
    setutxent();
    for (ut = getutxent(); ut != NULL; ut = getutxent()) {

        // skip for other logged type
        if (ut->ut_type != USER_PROCESS)
            continue;

        // convert data
        if ((t = localtime((time_t *) &ut->ut_tv.tv_sec)) == NULL)
            exit_err("gmtime");
        strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", t);

        // show information in one line
        printf("%-16s %-32s %-32s\n", ut->ut_user, ut->ut_line, timestr);
    }
    endutxent();

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "40-4";

    printf("\nUSAGE:\n\n");
    printf("%s\n", exename);
    printf("%s -h\n\n", exename);
}
