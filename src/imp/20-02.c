/*
SYNOPSIS

    Solution for exercise 20-02.

REQUIREMENT

    Write a program that show that when the dispotion of a pending signal is
    changed to be SIG_IGN, the program never sees (catches) the signal.

SOLUTION

    Unimplemented

USAGE

    $ ./dest/bin/20-02 &
    [1] 22399

    # nothing happen because process ignores SIGUSR1
    $ kill -s SIGUSR1 22399

    # still catch SIGUSR2 because process doesn't ignore it
    $ kill -s SIGUSR2 22399
    received signal: 12 - User defined signal 2
    [1]+  Done                    ./dest/bin/20-02

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

#include <tlpiexer/error.h>

void sig_handler(int sig);

int main(int argc, char **argv)
{
    sigset_t sigset;
    int sig;

    if (signal(SIGUSR1, SIG_IGN) == SIG_ERR)
        exit_err("signal:SIGUSR1");
    if (signal(SIGUSR2, sig_handler) == SIG_ERR)
        exit_err("signal:SIGUSR2");

    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigaddset(&sigset, SIGUSR2);
    if (sigwait(&sigset, &sig) == -1)
        exit_err("sigwait");

    printf("received signal: %i - %s\n", sig, strsignal(sig));

    return EXIT_SUCCESS;
}

void sig_handler(int sig)
{
    // do nothing, just catch signal to avoid terminate
}
