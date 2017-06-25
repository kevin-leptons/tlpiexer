/*
SYNOPSIS

    Solution for excercise 23-03.

REQUIREMENT

    Write a program to show that if the evp argument to 
    timer_create() is specified as NULL, then this is quivalent to specifying 
    evp as a pointer to a sigevent structure sigev_nodify set to SIGEV_SIGNAL,
    sigev_signo set to SIGALRM, and si_value.sival_int set to the timer id.

SOLUTION

    - setup timer by call timer_create(), timer_settime()
    - setup signal hanler by call sigaction()
    - sighandler() must catch SIGALRM then show correct timer identity
      and value of info->si_value.sival_int

USAGE

    $ ./dest/bin/23-03
    timerid = 29270032
    set timer for 3 seconds
    catch Alarm clock
    info->si_value.sival_int = 29270032

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#include <tlpiexer/error.h>

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : callback will be call when process has received signal
// arg      :
//  - sig: signal number
//  - info: information about signal
//  - ucontext: ?
// ret      : none
void sighanlder(int sig, siginfo_t *info, void *ucontext);

int main(int argc, char **argv)
{
    // variables
    int opt;
    struct sigaction sigact;
    timer_t timerid;
    struct itimerspec itimer;

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
    if (optind > argc) {
        showhelp();
        return EXIT_FAILURE;
    }

    // setup handler for SIGALRM 
    sigact.sa_sigaction = sighanlder;
    sigact.sa_flags = SA_SIGINFO;
    sigemptyset(&sigact.sa_mask);
    if (sigaction(SIGALRM, &sigact, NULL) == -1)
        exit_err("sigaction");

    // create timer
    if (timer_create(CLOCK_REALTIME, NULL, &timerid) == -1)
        exit_err("timer_create");

    // show timer id
    printf("timerid = %li\n", (long) timerid);

    // set timer
    itimer.it_interval.tv_sec = 0;
    itimer.it_interval.tv_nsec = 0;
    itimer.it_value.tv_sec = 3;
    itimer.it_value.tv_nsec = 0;
    if (timer_settime(timerid, 0, &itimer, NULL) == -1)
        exit_err("timer_settime");
    printf("set timer for %li seconds\n", itimer.it_value.tv_sec);

    // wait until timer has expired
    sleep(4);

    // delete timer
    if (timer_delete(timerid) == -1)
        exit_err("timer_delete");

    // success
    return EXIT_SUCCESS;
}

void sighanlder(int sig, siginfo_t *info, void *ucontext)
{
    // unsafe
    printf("catch %s\n", strsignal(sig));
    printf("info->si_value.sival_int = %i\n", info->si_value.sival_int);
}

void showhelp()
{
    // variables
    char exename[] = "22-3";

    printf("\nUSAGE:\n\n");
    printf("%s\n", exename);
    printf("%s -h\n\n", exename);
}
