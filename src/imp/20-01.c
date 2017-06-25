/*
SYNOPSIS

    Solution for exercise 20-01.

REQUIREMENT

    As noted in section 20.3, sigaction() is more portable than signal() for
    establishing a signal handler. Replace the use of signal() by sigaction()
    in the program in listing 20-7 (sig_receive.c).

SOLUTION

    - copy source code in tlpi/signals/sig_receiver.c
    - replace call signal() by sigaction()

USAGE

    TERMINAL 1

    $ ./dest/bin/20-01 &
    [1] 11266
    $ fg
    signal 10 - User defined signal 1 caught 2 times
    signal 12 - User defined signal 2 caught 1 times

    TERMINAL 2

    $ kill -s SIGUSR1 11266
    $ kill -s SIGUSR1 11266
    $ kill -s SIGUSR2 11266

AUTHORS

    Michael Kerrisk
    Kevin Leptons <kevin.leptons@gmail.com>
*/

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include <tlpiexer/error.h>

// spec     : this is global variable use for signal handler
static int sig_count[NSIG];
static volatile sig_atomic_t got_sigint = 0;

// spec     : handle for signal
// arg      :
//  - sig: signal number
// ret      : none
static void sig_handle(int sig);

// spec     : print sigset to stdout
// arg      :
//  - prefix: string will print before each signal string
//  - sigset: structure contains set of signals
// ret      : none
void show_sigset(const char *prefix, const sigset_t *sigset);

int main(int argc, char **argv)
{
    // variables
    int n, nsecs;
    sigset_t pending_mask, blocking_mask, empty_mask;
    struct sigaction sigact;

    // init
    sigact.sa_flags = SA_SIGINFO;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_handler = sig_handle;

    for (n = 1; n < NSIG; n++)
        (void) sigaction(n, &sigact, NULL);

    if (argc > 1) {
        nsecs = atoi(argv[1]);

        sigfillset(&blocking_mask);
        if (sigprocmask(SIG_SETMASK, &blocking_mask, NULL) == -1)
            exit_err("sigprocmask");

        printf("%s: sleeping for %d seconds\n", argv[0], nsecs);
        sleep(nsecs);

        if (sigpending(&pending_mask) == -1)
            exit_err("sigpending");

        printf("%s: pending signals are: \n", argv[0]);
        show_sigset("\t\t", &pending_mask);

        sigemptyset(&empty_mask);
        if (sigprocmask(SIG_SETMASK, &empty_mask, NULL) == -1)
            exit_err("sigprocmask");
    }

    while (!got_sigint)
        continue;

    for (n = 1; n < NSIG; n++)
        if (sig_count[n] != 0)
            printf("%s: signal %d - %s caught %d times \n", argv[0],
                   n, strsignal(n), sig_count[n]);

    // success
    return EXIT_SUCCESS;
}

static void sig_handle(int sig)
{
    if (sig == SIGINT)
        got_sigint = 1;
    else
        sig_count[sig]++;
}

void show_sigset(const char *prefix, const sigset_t *sigset)
{
    // variables
    int sig, cnt;

    // init
    cnt = 0;

    // show
    for (sig = 1; sig < NSIG; sig++) {
        if (sigismember(sigset, sig)) {
            cnt++;
            printf("%s%d (%s)\n", prefix, sig, strsignal(sig));
        }
    }

    if (cnt == 0)
        printf("%s<empty signal set>\n", prefix);
}
