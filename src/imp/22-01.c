/*
SYNOPSIS

    Solution for exercise 22-01.

REQUIREMENT

    Section 22.2 noted that if a stopped process that has established a handler
    for and blocked SIGCONT is later resumed as a consequence of receving a
    SIGCONT, then the handler is invoked only when SIGCONT is unblocked.
    Write a program to verify this. Recal that a process can be stopped by
    typing the terminal supend character (usually control-z) and can be sent a
    SIGCONT signal using the command kill -CONT (or implicitly, using the shell
    fg command).

SOLUTION

    Setup handler for SIGCONT and block SIGCONT.

    If -u is set, after received SIGCONT, unblock SIGCONT. So signal handler
    will be catch with SIGCONT.

USAGE

    CASE 1: Don't unblock SIGCONT

    $ ./dest/bin/22-01
    process id: 19019
    block 'Continued'

    [1]+  Stopped                 ./dest/bin/22-0
    $ kill -s SIGCONT 19019
    process has continue

    CASE 2: Unblock SIGCONT

    $ ./dest/bin/22-01 -u
    process id: 19030
    block 'Continued'
    [1]   Done                    ./dest/bin/22-01

    [2]+  Stopped                 ./dest/bin/22-01 -u

    $ kill -s SIGCONT 19030
    process has continue
    unblock 'Continued'
    handle for 'Continued'

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include <tlpiexer/error.h>

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : callback function when process received signal
// arg      :
//  - sig: signal number
// ret      : none
void sighandler(int sig);

int main(int argc, char **argv)
{
    // variables
    int opt;
    int ublk_sigcont;
    sigset_t sigmask;
    struct sigaction sigact;

    // init
    ublk_sigcont = 0;

    // parse options
    while ((opt = getopt(argc, argv, "hu")) != -1) {
        switch (opt) {
            case 'u':
                ublk_sigcont = 1;
                break;
            case 'h':
                showhelp();
                return EXIT_SUCCESS;
            default:
                showhelp();
                return EXIT_FAILURE;
        }
    }

    // verify no argument is provided
    if (optind > argc) {
        showhelp();
        return EXIT_FAILURE;
    }

    // show information about this process
    printf("process id: %i\n", getpid());

    // setup SIGCONT handler
    sigact.sa_handler = sighandler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    if (sigaction(SIGCONT, &sigact, NULL) == -1)
        exit_err("sigaction");

    // block SIGCONT
    printf("block '%s'\n", strsignal(SIGCONT));
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGCONT);
    if (sigprocmask(SIG_BLOCK, &sigmask, NULL) == -1)
        exit_err("sigprocmask");
    
    // stop process
    // other process can send SIGCONT to this process
    raise(SIGSTOP);

    // process continue by received SIGCONT
    printf("process has continue\n");
    
    // unblock SIGCONT
    if (ublk_sigcont) {
        printf("unblock '%s'\n", strsignal(SIGCONT));

        if (sigprocmask(SIG_UNBLOCK, &sigmask, NULL) == -1)
            exit_err("sigprocmask");
    }

    // success
    return EXIT_SUCCESS;
}

void sighandler(int sig)
{
    // unsafe
    printf("handle for '%s'\n", strsignal(sig));
}

void showhelp()
{
    // variables
    char exename[] = "22-1";

    printf("\nUSAGE:\n\n");
    printf("\t%s [-u]\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("OPTIONS:\n\n");
    printf("\t-b: block SIGCONT before setup signal handler for that\n");
    printf("\t-u: unblock SIGCONT after process received SIGCONT\n\n");
}
