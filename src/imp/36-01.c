/*
SYNOPSIS

    Solution for exercise 36-01.

REQUIREMENT

    Write a program that shows that the getrusage() RUSAGE_CHILDREN flag
    retrieves information about only the children for which a wait() call has
    been performed (Have the program create child process that consumes
    some cpu time, and then have the parent call getrusage() before and after
    calling wait()).

SOLUTION

    - parse arguments
    - create child process. in child process, consume cpu time by call times().
      child process exit after CONSUMES_TIME seconds on cpu time
    - during that, parent call getrusage() to retrieve resource used by
      child process
    - parent wait for children exited, then call getrusage() again to retrieve
      resource used by child process

USAGE

    $ ./dest/bin/36-01
    retrieve child during running
    cpu time            : 0 secs 0 usecs
    system time         : 0 secs 0 usecs
    resident set size   : 0 kb
    page faults non i/o : 0
    page faults i/o     : 0
    retrieve after child exited
    cpu time            : 6 secs 0 usecs
    system time         : 44 secs 968000 usecs
    resident set size   : 632 kb
    page faults non i/o : 20
    page faults i/o     : 0

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <sys/wait.h>

#include <tlpiexer/error.h>

#define CONSUMES_TIME 6

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : show information of resource usage
// arg      :
//  - usage: pointer to resource usage structure
// ret      : none
void dump_rusage(struct rusage *usage);

int main(int argc, char *argv[])
{
    // variables
    int opt;
    struct tms t;
    int clock_tick;
    struct rusage usage;

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

    // verify no arguments has provided
    if (argc > 1) {
        showhelp();
        return EXIT_FAILURE;
    }

    // init
    clock_tick = sysconf(_SC_CLK_TCK);

    // create child process then retrieve resouces is used by that
    switch (fork()) {
        case -1:
            // fail
        case 0:
            // child process continue here
            
            // consume cpu time and exit after 
            // CONSUMES_TIME on seconds cpu time
            t.tms_utime = 0;
            for (; t.tms_utime/clock_tick < CONSUMES_TIME;) 
                if (times(&t) == -1)
                    exit_err("times");

            // sucess
            _exit(EXIT_SUCCESS);
        default:
            // parent process continue here
            
            // wait for child process setup
            sleep(3);

            // retrieve resources is used by child processes
            if (getrusage(RUSAGE_CHILDREN, &usage) == -1)
                exit_err("getrusage");
            printf("retrieve child during running\n");
            dump_rusage(&usage);

            // wait for child process exited
            wait(NULL);

            // retrieve resource isused by child processes again
            if (getrusage(RUSAGE_CHILDREN, &usage) == -1)
                exit_err("getrusage");
            printf("retrieve after child exited\n");
            dump_rusage(&usage);

            // success
            return EXIT_SUCCESS;
            
    }
}

void showhelp()
{
    // variables
    char exename[] = "36-1";

    printf("\nUSAGE:\n\n");
    printf("%s\n", exename);
    printf("%s -h\n\n", exename);
}

void dump_rusage(struct rusage *usage)
{
    printf("%-20s: %ld secs %ld usecs\n", "cpu time",
            usage->ru_utime.tv_sec, usage->ru_utime.tv_usec);
    printf("%-20s: %ld secs %ld usecs\n", "system time",
            usage->ru_stime.tv_sec, usage->ru_stime.tv_usec);
    printf("%-20s: %ld kb\n", "resident set size", usage->ru_maxrss);
    printf("%-20s: %ld\n", "page faults non i/o", usage->ru_minflt);
    printf("%-20s: %ld\n", "page faults i/o", usage->ru_majflt);
}
