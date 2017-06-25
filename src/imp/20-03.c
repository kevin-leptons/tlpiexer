/*
SYNOPSIS

    Solution for exercise 20-03.

REQUIREMENT

    Write programs that verify the effect of the SA_RESETHAND and SA_NODEFER
    flags when establishing a signal handler with sigaction().

SOLUTION

    - parse options
    - verify arguments, options
    - switch options to call sigaction() with flags
    - call raise() to see behavior of signal handler

    If use SA_RESETHAND, signal will be catch 1 time, then process terminate.
    If use SA_NODEFER, signal will be catch many times.

USAGE

    $ ./dest/bin/20-03 -u r
    raise Interrupt
    catch Interrupt
    raise Interrupt

    $ ./dest/bin/20-03 -u n
    raise Interrupt
    catch Interrupt
    raise Interrupt
    catch Interrupt
    raise Interrupt
    catch Interrupt
    raise Interrupt
    catch Interrupt
    raise Interrupt
    catch Interrupt
    raise Interrupt
    catch Interrupt
    raise Interrupt
    catch Interrupt
    raise Interrupt
    catch Interrupt
    raise Interrupt 8 times, catch 8 times

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#define _GNU_SOURCE 

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include <tlpiexer/error.h>

#define RET_SUCCESS 0
#define RET_FAILURE -1
#define SIG_TEST SIGINT

// spec     : this is variable use for signal handler
int ncatch;

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : setup signal handler use SA_RESETHAND
// arg      : none
// ret      : 
//  - 0 on success
//  - -1 on error
int setup_resethand();

// spec     : setup signal handler use SA_NODEFER
// arg      : none
// ret      :
//  - 0 on success
//  - -1 on error
int setup_nodefer();

// spec     : handler for SIG_TEST, this function increase ncatch by one
// arg      :
//  - sig: signal number
void signal_handler(int sig);

int main(int argc, char **argv)
{
    // variables
    int opt;
    char use;
    int nraise;
    int i;

    // init
    nraise = 8;
    ncatch = 0;

    // parse options
    while ((opt = getopt(argc, argv, "hu:")) != -1) {
        switch (opt) {
            case 'u':
                use = optarg[0];
                break;
            case 'h':
                showhelp();
                return EXIT_SUCCESS;
            default:
                showhelp();
                return EXIT_FAILURE;
        }
    }

    // verify options
    if (optind > argc) {
        showhelp();
        return EXIT_FAILURE;
    }

    // setup handler
    switch (use) {
        case 'r':
            if (setup_resethand() == -1)
                exit_err("setup_resethand");
            break;
        case 'n':
            if (setup_nodefer() == -1)
                exit_err("setup_nodefer");
            break;
        default:
            showhelp();
            return EXIT_FAILURE;
    }
    
    // try init events to see how SA_RESETHAND and SA_NODEFER work
    for (i = 0; i < nraise; i++) {
        printf("raise %s\n", strsignal(SIG_TEST));
        if (raise(SIG_TEST) == -1)
            exit_err("raise");
    }

    // show different
    printf("raise %s %i times, catch %i times\n", 
            strsignal(SIG_TEST), nraise, ncatch);

    // success
    return EXIT_SUCCESS;
}

int setup_resethand()
{
    // variables
    struct sigaction sigact;

    // init
    sigact.sa_handler = signal_handler;
    if (sigemptyset(&sigact.sa_mask) == -1)
        return RET_FAILURE;
    sigact.sa_flags = SA_RESETHAND;

    // add handler
    if (sigaction(SIG_TEST, &sigact, NULL) == -1)
        return RET_FAILURE;

    // success
    return RET_SUCCESS;
}

int setup_nodefer()
{
    // variables
    struct sigaction sigact;

    // init
    sigact.sa_handler = signal_handler;
    if (sigemptyset(&sigact.sa_mask) == -1)
        return RET_FAILURE;
    sigact.sa_flags = SA_NODEFER;

    // add handler
    if (sigaction(SIG_TEST, &sigact, NULL) == -1)
        return RET_FAILURE;

    // success
    return RET_SUCCESS;
}

void signal_handler(int sig)
{
    ncatch++;

    // unsafe
    printf("catch %s\n", strsignal(sig));
}

void showhelp()
{
    // variables
    char exename[] = "20-3";

    printf("\nUSAGE:\n\n");
    printf("\t%s -u [r | n]\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("OPTIONS:\n\n");
    printf("\t-u: switch call sigaction() with flag.\n");
    printf("\t    r = SA_RESETHAND, n = SA_NODEFER\n\n");
}
