/*
SYNOPSIS

    Solution for exercise 21-01.

REQUIREMENT

    Implement abort().

SOLUTION

    Implement abort()

        - call raise(SIGABRT) to abort process
        - if process still continue, reset signal handler of SIGABRT to SIG_DFL
        - call raise(SIGABRT) again to abort process

    Implement main()

        - do something
        - call abort()

USAGE

    # check core dump file is allowed to create or not
    # if returned value is 0, core dump file is not allowed to create
    # if returned value is positive, core dump file is allowd to created
    # with maximun size equal returned value
    $ ulimit -c

    # allow core dump file to create with maximun size equal 10,000 bytes
    $ sudo ulimit -c 10000

    # run process then abort
    $ ./dest/bin/21-01
    $ ls -l core
    -rw------- 1 kevin kevin 344064 Jun 15 14:10 core

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : create core file, contains data of virtual memory until abort 
//            time then terminate process. it is use to debug
// arg      : none
// ret      : none
void abort_x(void);

int main(int argc, char **argv)
{
    // variables
    int opt;

    // parse options
    while ((opt = getopt(argc, argv, "h")) != -1) {
        switch (opt) {
            case 'h':
                showhelp();
                return EXIT_SUCCESS;
            default:
                showhelp();
                return EXIT_FAILURE;
        }
    }

    // verify no argument provided
    if (argc > 1) {
        showhelp();
        return EXIT_FAILURE;
    }

    // abort
    printf("abort\n");
    abort_x();

    // abort does not success
    printf("abort is not success, process still continue\n");

    // success
    return EXIT_SUCCESS;
}

void abort_x(void)
{
    // variables
    struct sigaction sigact;

    // try sent SIGABRT
    // if signal handler abort process normal, done
    raise(SIGABRT);

    // signal handler for SIGABRT is not abort, reset handler to SIG_DFL
    sigact.sa_handler = SIG_DFL;
    if (sigemptyset(&sigact.sa_mask) == -1)
        exit(errno);
    sigact.sa_flags = 0;
    if (sigaction(SIGABRT, &sigact, NULL) == -1)
        exit(errno);

    // raise SIGABRT again, SIG_DFL will abort process
    raise(SIGABRT);
}

void showhelp()
{
    // variables
    char exename[] = "20-1";

    printf("\nUSAGE:\n\n");
    printf("%s\n", exename);
    printf("%s -h\n\n", exename);
}
