/*
SYNOPSIS

    Solution for exercise 23-04.

REQUIREMENT

    Modify the program in listing 23-5 (ptmr_sigev_signal.c) to use
    sigwaitinfo() intead of a signal handler.

SOLUTION

    - copy source code in tlpi/timers/ptmr_sigev_signal.c
    - reqplace use of signal handler by sigwaitinfo()

USAGE

    $ ./dest/bin/23-04 3 7 10
    catch 'Real-time signal 30'
    siginfo.si_value.sival_ptr = 0x7ffd4dded850
    timer_getoverrun() = 0
    catch 'Real-time signal 30'
    siginfo.si_value.sival_ptr = 0x7ffd4dded858
    timer_getoverrun() = 0
    catch 'Real-time signal 30'
    siginfo.si_value.sival_ptr = 0x7ffd4dded860
    timer_getoverrun() = 0

AUTHORS

    Michael Kerrisk
    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>

#include <tlpiexer/error.h>

#define TIMER_MAX 8

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

int main(int argc, char **argv)
{
    // variables
    int opt;
    int i;
    struct sigevent sigev;
    timer_t timerids[TIMER_MAX];
    struct itimerspec itimer;
    sigset_t sigmask;
    int nsig;
    siginfo_t siginfo;

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

    // verify arguments
    if (argc < 2) {
        showhelp();
        return EXIT_FAILURE;
    }

    // create default of iteration timer
    itimer.it_interval.tv_sec = 0;
    itimer.it_interval.tv_nsec = 0;
    itimer.it_value.tv_nsec = 0;

    // setup signal event
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGRTMAX;

    // block signal which using to alarm
    // so during setup if any timer expired, it will save to queu then
    // after unblock it can catch by sigwaitinfo()
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGRTMAX);
    if (sigprocmask(SIG_BLOCK, &sigmask, NULL) == -1)
        exit_err("sigprocmask");

    // setup timer for each time value from command line
    for (i = 0; i < argc - 1; i++) {

        // create timer
        sigev.sigev_value.sival_ptr = timerids + i;
        if (timer_create(CLOCK_REALTIME, &sigev, &timerids[i]) == -1)
            exit_err("timer_create");
        
        // set timer
        itimer.it_value.tv_sec = atoi(argv[i + 1]);
        if (timer_settime(timerids[i], 0, &itimer, NULL) == -1)
            exit_err("timer_settime");
    }

    // wait for signal 
    // break when cat enough signal
    for (nsig = 0; nsig < argc - 1; nsig++) {
        if (sigwaitinfo(&sigmask, &siginfo) == -1)
            exit_err("sigwaitinfo");

        // show infomation of signal just received
        printf("catch '%s'\n", strsignal(siginfo.si_signo));
        printf("siginfo.si_value.sival_ptr = %p\n", 
                siginfo.si_value.sival_ptr);
        printf("timer_getoverrun() = %i\n", 
                timer_getoverrun((timer_t*)siginfo.si_value.sival_ptr));
    }

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "23-4";

    printf("\nUSAGE:\n\n");
    printf("\t%s timeval...\n\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("ARGUMENTS:\n\n");
    printf("\ttimeval...: list of timeval, each value will call with\n");
    printf("\t            timer_settime()\n\n");

}
