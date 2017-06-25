/*
SYNOPSIS

    Solution for exercise 23-01.

REQUIREMENT

    Although alarm() is implemented as a system call within the linux kernel,
    this is redundant. Implement alarm() using setitimer().

SOLUTION

    Implement alarm()

        - call setitimer() with specific arguments

    Implement main()

        - parse options
        - verfiy arguments, options
        - setup signal handler for SIGALRM
        - call alarm()

USAGE

    $ ./dest/bin/23-01 -t 3
    catch Alarm cloc

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : set a timer by seconds. when timer has expired, send SIGALR
//            to current process
// arg      :
//  - seconds: time before timer has expired
// ret      : time remaining of pre-call alarm()
unsigned int alarm(unsigned int seconds);

// spec     : callback function will be call if process received signal
// arg      :
//  - sig: signal number
// rert     : none
void sighandler(int sig);

int main(int argc, char **argv)
{
    // variables
    int opt;
    int secs;
    struct sigaction sigact;

    // init
    secs = -1;

    // parse options
    while ((opt = getopt(argc, argv, "ht:")) != -1) {
        switch (opt) {
            case 't':
                secs = atoi(optarg);
                break;
            case 'h':
                showhelp();
                return EXIT_SUCCESS;
            default:
                showhelp();
                return EXIT_FAILURE;
        }
    }

    // verify no argument has provided
    // verify options
    if (optind > argc || secs == -1) {
        showhelp();
        return EXIT_FAILURE;
    }

    // setup signal handler for SIGALRM
    sigact.sa_handler = sighandler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    if (sigaction(SIGALRM, &sigact, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    // setup timer
    alarm(secs);

    // wait for SIGALRM
    sleep(secs + 1);

    // success
    return EXIT_SUCCESS;
}

unsigned int alarm(unsigned int seconds)
{
    // variables
    struct itimerval exptime;
    struct itimerval old_exptime;

    // init
    exptime.it_interval.tv_sec = 0;
    exptime.it_interval.tv_usec = 0;
    exptime.it_value.tv_sec = seconds;
    exptime.it_value.tv_usec = 0;

    // set timer
    if (setitimer(ITIMER_REAL, &exptime, &old_exptime) == -1)
        return 0;

    // result
    return old_exptime.it_value.tv_sec;
}

void sighandler(int sig)
{
    // unsafe
    printf("catch %s\n", strsignal(sig));
}
void showhelp()
{
    // variables
    char exename[] = "23-1";

    printf("\nUSAGE:\n\n");
    printf("\t%s -t time\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("OPTIONS:\n\n");
    printf("\t-t: time to call alarm()\n\n");
}
