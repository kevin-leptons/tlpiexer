/*
SYNOPSIS

    Solution for exercise 30-01.

REQUIREMENT

    Modify the program in listing 30-1 (thread_incr.c) so that each loop in
    the thread's start function outputs the current value of glob and some
    identifier that uniquely identifies the thread. The unique identifier for
    the thread could be specified as an argument to the pthread_create()
    call used to create te thread. For this program, that would require 
    changing the argument of the thread's start function to be a
    pointer to a structure containing the unique identifier and a loop limit
    value. Run the program, redirecting output to a file, and then inspect
    the file to see what happens to glob as the kernel alternates execution 
    between the two threads.

SOLUTION

    - copy source code in tlpi/threads/thread_incr.c
    - modify to meet requirements

USAGE

    $ ./dest/bin/30-01 -t 1000000
    ...
    thread_t=0x7ffc4fba9750; glob_incvar=1999995
    thread_t=0x7ffc4fba9750; glob_incvar=1999996
    thread_t=0x7ffc4fba9750; glob_incvar=1999997
    thread_t=0x7ffc4fba9750; glob_incvar=1999998
    global inc-var: 1999999
    
AUTHOR

    Michael Kerrisk
    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include <tlpiexer/error.h>

// spec     : global increase varialble
static int glob_incvar = 0;

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : specify data structure for thread_entry() argument
struct pthread_entry_arg
{
    pthread_t thread;
    int ninc;
};

// spec     : function start of thread
// arg      :
//  - arg: argument as void pointer, but pthread_create() must specify
//    structure thread_entry_arg when create thread
// ret      : void pointer to non-stacck memory
static void*
thread_entry(void *arg);

int main(int argc, char **argv)
{
    // variables
    int opt;
    int ninc;
    struct pthread_entry_arg t1_arg, t2_arg;

    // init
    ninc = 100000;

    // parse options
    while ((opt = getopt(argc, argv, "ht:")) != -1) {
        switch (opt) {
            case 't':
                ninc = atoi(optarg);
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
    // verify options
    if (optind > argc || ninc == 0) {
        showhelp();
        return EXIT_FAILURE;
    }

    // prepare arguments
    t1_arg.ninc = ninc;
    t2_arg.ninc = ninc;

    // create thread to increase global variable
    if (pthread_create(&t1_arg.thread, NULL, thread_entry, &t1_arg) != 0)
        exit_err("pthread_create:2");
    if (pthread_create(&t2_arg.thread, NULL, thread_entry, &t2_arg) != 0)
        exit_err("pthread_create:1");

    // wait for thread terminate
    if (pthread_join(t1_arg.thread, NULL) != 0)
        exit_err("pthread_join:1");
    if (pthread_join(t2_arg.thread, NULL) != 0)
        exit_err("pthread_join:2");

    // show increase variable
    printf("global inc-var: %d\n", glob_incvar);

    // success
    return EXIT_SUCCESS;
}

static void*
thread_entry(void *arg)
{
    // variables
    struct pthread_entry_arg *edata;
    int tmp, i;

    // init
    edata = (struct pthread_entry_arg*)arg;

    // increase template variable
    // and save it again
    for (i = 0; i < edata->ninc; i++) {
        printf("thread_t=%p; glob_incvar=%i\n", &edata->thread, glob_incvar);

        tmp = glob_incvar;
        tmp++;
        glob_incvar = tmp;
    }

    // success
    return NULL;
}

void showhelp()
{
    // variables
    char exename[] = "30-1";

    printf("\nUSAGE:\n\n");
    printf("%s -t n\n", exename);
    printf("%s -h\n\n", exename);
}
