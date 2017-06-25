/*
SYNOPSIS

    Solution for exercise 27-06.

REQUIREMENT

    Suppose that a parent process has estabilshed a handler for SIGCHLD and
    also blocked this signal. Subsequently, one of its children exits, and
    the parent then does a wait() to collect the child's status. What happens
    when the parent unblocks SIGCHLD? write a program to verify your answer.
    What is the relevance of result for a program calling the system()
    function?.

SOLUTION

    - add handler for SIGCHLD. handler use wait() to get status of 
      child process
    - block SIGCHLD depend on options -b
    - create child processes
    - child process do something then exit
    - parent call wait() to get status of child process

USAGE

    $ ./dest/bin/27-06
    child: #0; pid=27328
    child: #1; pid=27329
    hand(): catch pid=27328; sig=17; status=1
    child: #2; pid=27330
    child: #3; pid=27331
    child: #4; pid=27333
    wait(): catch pid=27328; status=256
    child: #9; pid=27338
    child: #7; pid=27336
    child: #8; pid=27337
    child: #6; pid=27335
    child: #5; pid=27334
    hand(): catch pid=27329; sig=17; status=1
    wait(): catch pid=27329; status=256
    hand(): catch pid=27330; sig=17; status=1
    wait(): catch pid=27330; status=256
    hand(): catch pid=27331; sig=17; status=1
    wait(): catch pid=27331; status=256
    hand(): catch pid=27333; sig=17; status=1
    wait(): catch pid=27333; status=256
    hand(): catch pid=27334; sig=17; status=1
    wait(): catch pid=27334; status=256
    hand(): catch pid=27335; sig=17; status=1
    wait(): catch pid=27335; status=256
    hand(): catch pid=27336; sig=17; status=1
    wait(): catch pid=27336; status=256
    hand(): catch pid=27337; sig=17; status=1
    wait(): catch pid=27337; status=256
    hand(): catch pid=27338; sig=17; status=1
    wait(): catch pid=27338; status=256

    $ ./dest/bin/27-06 -b
    child: #0; pid=27360
    child: #1; pid=27361
    child: #4; pid=27364
    child: #5; pid=27365
    child: #6; pid=27366
    child: #8; pid=27368
    child: #7; pid=27367
    wait(): catch pid=27360; status=256
    child: #3; pid=27363
    child: #9; pid=27369
    child: #2; pid=27362
    wait(): catch pid=27361; status=256
    wait(): catch pid=27362; status=256
    wait(): catch pid=27363; status=256
    wait(): catch pid=27364; status=256
    wait(): catch pid=27365; status=256
    wait(): catch pid=27366; status=256
    wait(): catch pid=27367; status=256
    wait(): catch pid=27368; status=256
    wait(): catch pid=27369; status=256

<cmd> [-b] [-h]
do same thing as requirements. if -b is set, block signal before create
child process

<cnd> -h
show help information

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#include <tlpiexer/error.h>

#define NUM_CHILD_PROC 10

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : callback will be call when process received signal
// arg      :
//  - sig: signal number
//  - info: information about signal
//  - ucontext: ?
// ret      : none
void sighandler(int sig, siginfo_t *info, void *ucontext);

int main(int argc, char **argv)
{
    // variables
    int opt;
    sigset_t sigmask;
    struct sigaction sigact;
    int i;
    pid_t cid;
    int status;
    int sigblk;

    // init
    sigblk = 0;

    // parse options
    while ((opt = getopt(argc, argv, "hb")) != -1) {
        switch (opt) {
            case 'b':
                sigblk = 1;
                break;
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

    // block SIGCHLD
    if (sigblk) {
        sigemptyset(&sigmask);
        sigaddset(&sigmask, SIGCHLD);
        if (sigprocmask(SIG_BLOCK, &sigmask, NULL) == -1) 
            exit_err("sigprocmask");
    }

    // set SIGCHLD handler
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = SA_SIGINFO;
    sigact.sa_sigaction = sighandler;
    if (sigaction(SIGCHLD, &sigact, NULL) == -1)
        exit_err("sigaction");

    // create child processes
    for (i = 0; i < NUM_CHILD_PROC; i++) {
        switch (fork()) {
            case -1:
                exit_err("fork");
            case 0:
                // child process continue here

                // assume that child process do something here
                // to make different eixt between child processes
                printf("child: #%i; pid=%ld\n", i, (long)getpid());
                sleep(i);

                // success
                _exit(1);
            default:
                // parent process continue here

                // do notthing
                // continue create child process
                break;
        }
    }

    // wait for on child process exit
    for (;;) {
        cid = wait(&status);
        if (cid == -1) {
            if (errno == ECHILD)
                break;
            else
                exit_err("wait");
        }

        printf("wait(): catch pid=%ld; status=%i\n", (long)cid, status);
    }

    // success
    return EXIT_SUCCESS;
}

void sighandler(int sig, siginfo_t *info, void *ucontext)
{
    // unsafe
    printf("hand(): catch pid=%i; sig=%i; status=%i\n", 
            info->si_pid, sig, info->si_status);
}
void showhelp()
{
    // variables
    char exename[] = "27-6";

    printf("\nUSAGE:\n\n");
    printf("\t%s -b\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("OPTIONS:\n\n");
    printf("\t-b: block signal before create child\n\n"); 
}
