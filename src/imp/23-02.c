/*
SYNOPSIS

    Solution for exercise 23-02.

REQUIREMENT

    Try running the program in listing 23-3 (t_nanosleep.c) in the background
    with a 60-second sleep interval, while using the following command to send
    as many SIGINT signal as possible to the background process.

        $ while true: do kill -INT pid; done

    You should observe that the program sleeps rather longer than expected.
    Replace the use of nanosleep() with the use of clock_gettime()
    (use a CLOCK_REALTIME clock) and clock_nanosleep() with the TIMER_ABSTIME
    flag (This exercise require linux 2.6). Repeat the test with the modified
    program and explain the difference.

SOLUTION

    - compile and run program in tlpi/timers/t_nanosleep.c. save time 
      processing
    - copy source code from tlpi/timers/t_nanosleep.c
    - replace function as requirements
    - compile and run new program. save time processing
    - explain different between time processing of two program

USAGE
    
    $ ./dest/bin/23-02 -s 20 -u 0 &
    [1] 1930

    # run this command about 6 seconds then Ctrl + C to break
    $ while true; do kill -INT 1930; done 

    $ fg
    ./dest/bin/23-02 -s 20 -u 0
    sleep for 20 secs 63 usecs

AUTHORS
    Michael Kerrisk
    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#include <tlpiexer/error.h>

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : cacllback handler will be call when process received signal
// arg      :
//  - sig: signal number
// ret      : none
void sighandler(int sig);

int main(int argc, char **argv)
{
    // variables
    int opt;
    struct timeval stime, ftime;
    struct timespec req;
    struct sigaction sigact;
    int secs, usecs;
    int r;

    // init
    secs = -1;
    usecs = 0;

    // parse options
    while ((opt= getopt(argc, argv, "hs:u:")) != -1) {
        switch (opt) {
            case 's':
                secs = atoi(optarg);
                break;
            case 'u':
                usecs = atoi(optarg);
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

    // setup SIGINT handler
    // to allow interrupt occur during sleep
    sigact.sa_handler = sighandler;
    sigact.sa_flags = 0;
    sigemptyset(&sigact.sa_mask);
    if (sigaction(SIGINT, &sigact, NULL) == -1)
        exit_err("sigaction");

    // save when start sleep
    if (gettimeofday(&stime, NULL) == -1)
        exit_err("gettimeofday");

    // get current clock
    // prepare for clock sleep
    if (clock_gettime(CLOCK_REALTIME, &req) == -1)
        exit_err("clock_gettime");
    req.tv_sec += secs;
    req.tv_nsec += usecs;

    // loop to complete sleep
    for (;;) {
        r = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &req, NULL);
        if (r == 0)
            break;
        else if (r == EINTR) 
            continue;
        else {
            exit_err("clock_nanosleep");
        }
    }

    // save when finish sleep
    if (gettimeofday(&ftime, NULL) == -1)
        exit_err("gettimeofday");

    // show statictist
    printf("sleep for %li secs %li usecs\n", 
            ftime.tv_sec - stime.tv_sec, ftime.tv_usec - stime.tv_usec);

    // success
    return EXIT_SUCCESS;
}

void sighandler(int sig)
{
}

void showhelp()
{
    // variables
    char exename[] = "4-1";

    printf("\nUSAGE:\n\n");
    printf("\t%s -s seconds -n nseconds\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("OPTIONS:\n\n");
    printf("\t-s: specify time to sleep in seconds\n");
    printf("\t-n: specify time to sleep in nano-seconds\n\n");
}
