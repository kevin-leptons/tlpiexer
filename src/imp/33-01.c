/*
SYNOPSIS

    Solution for excercise 33-01.

REQUIREMENT

    Write a program to demonstrate that different threads in the same process
    can have different sets of pending signals, as returned by sigpending().
    You can do this by using pthread_kill() to send different signals to two
    different threads that have blocked these signals, and then have each of
    the threads call sigpending() and display information about pending
    signals (You may find the functions in listing 20-4 useful).

SOLUTION

    This program have three thread.

    Thread 0 will create thread 1 and thread 2. then send signals to them 
    by order:

        - thread 1: T1_SIG_BLOCK, T1_SIG_WAIT
        - thread 2: T2_SIG_BLOCK, T2_SIG_WAIT

    Thread 1 block T1_SIG_BLOCK and wait T1_SIG_WAIT. When received 
    T1_SIG_WAIT, show pending signals. It must show T1_SIG_BLOCK.

    Thread 2 block T2_SIG_BLOCK and wait T2_SIG_WAIT. When received 
    T2_SIG_WAIT, show pending signals. it must show T2_SIG_BLOCK.

USAGE

    $ ./dest/bin/33-01
    wait for thread setup
    wait for thread show pending signals
    34. Real-time signal 0
    36. Real-time signal 2

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

#include <tlpiexer/error.h>

#define T1_SIG_BLOCK SIGRTMIN
#define T1_SIG_WAIT SIGRTMIN + 1
#define T2_SIG_BLOCK SIGRTMIN + 2
#define T2_SIG_WAIT SIGRTMIN + 3

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : this is function will call when start thread
// arg      :
//  - data: point to input data for thread
// ret      : result data from thread
void *thread_entry(void *data);

// spec     : show sigset_t as readable string
// arg      : 
//  - sigset: set of signals to show
// ret      : none
void showsigset(const sigset_t *sigset);

// spec     : function will be call when receive signal
// arg      :
//  - sig: signal number
// ret      : none
void sighandler(int sig);

// spec     : contains signal setup information
struct sigsetup
{
    int sigblock;
    int sigwait;
};

int main(int argc, char **argv)
{
    // variables
    int opt;
    pthread_t thread_1, thread_2;
    struct sigsetup sig_1, sig_2;

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

    // init 
    sig_1.sigblock = T1_SIG_BLOCK;
    sig_1.sigwait = T1_SIG_WAIT;
    sig_2.sigblock = T2_SIG_BLOCK;
    sig_2.sigwait = T2_SIG_WAIT;

    // start two thread
    if (pthread_create(&thread_1, NULL, thread_entry, (void*)&sig_1) != 0)
        exit_err("pthread_create:1");
    if (pthread_create(&thread_2, NULL, thread_entry, (void*)&sig_2) != 0)
        exit_err("pthread_create:2");

    // wait for thread setup
    printf("wait for thread setup\n");
    sleep(3);

    // send signals to thread 1
    if (pthread_kill(thread_1, sig_1.sigblock) == -1)
        exit_err("pthread_kill");
    if (pthread_kill(thread_1, sig_1.sigwait) == -1)
        exit_err("pthread_kill");

    // send signals to thread 2
    if (pthread_kill(thread_2, sig_2.sigblock) == -1)
        exit_err("pthread_kill");
    if (pthread_kill(thread_2, sig_2.sigwait) == -1)
        exit_err("pthread_kill");

    // wait for thread show pending signals
    printf("wait for thread show pending signals\n");
    sleep(3);

    // do not use pthread_join() to join thread
    // it must drop into infinite wait
    // all of thread will be stop after main thread terminate

    // success
    return EXIT_SUCCESS;
}

void *thread_entry(void *data)
{
    // variables
    sigset_t mask_block, mask_wait, mask_pend;
    struct sigaction sigact;
    int sig;
    struct sigsetup *sig_setup;

    // init
    sig_setup = (struct sigsetup*)data; 
    sigemptyset(&mask_block);
    sigaddset(&mask_block, sig_setup->sigblock);
    sigemptyset(&mask_wait);
    sigaddset(&mask_wait, sig_setup->sigwait);
    sigact.sa_handler = sighandler;
    sigact.sa_flags = 0;
    sigemptyset(&sigact.sa_mask);

    // setup signal handler
    // to avoid terminate process
    if (sigaction(sig_setup->sigblock, &sigact, NULL) == -1) {
        perror("sigaction");
        return NULL;
    }
    if (sigaction(sig_setup->sigwait, &sigact, NULL) == -1) {
        perror("sigaction");
        return NULL;
    }

    // block signal
    if (pthread_sigmask(SIG_BLOCK, &mask_block, NULL) == -1) {
        perror("sigprocmask");
        return NULL;
    }

    // wait signal to sure that one block signal has received
    if (sigwait(&mask_wait, &sig) == -1) {
        perror("sigwait");
        return NULL;
    }

    // show pending signals
    if (sigpending(&mask_pend) == -1) { 
        perror("sigpending");
        return NULL;
    } 
    showsigset(&mask_pend);

    // success
    return NULL;
}

void showsigset(const sigset_t *sigset)
{
    // variables
    int sig;

    // show each signals
    for (sig = 1; sig < NSIG; sig++)
        if (sigismember(sigset, sig)) {
            printf("%d. %s\n", sig, strsignal(sig));
        }
}

void sighandler(int sig)
{
    // do nothing
    // it will call to replace SIG_DFL for signal
    // to avoid terminate process
}

void showhelp()
{
    // variables
    char exename[] = "33-1";

    printf("\nUSAGE:\n\n");
    printf("%s\n", exename);
    printf("%s -h\n\n", exename);
}
