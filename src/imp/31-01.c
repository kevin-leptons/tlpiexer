/*
SYNOPSIS

    Solution for exercise 31-01.

REQUIREMENT

    Implement a function one_time_init(control, init), that performs the
    equivalent of pthread_once(). The control argument should be a pointer
    to a statically allocated structure containing a boolean variable and
    a mutex. The boolean variable indicates whether the function init already
    been called, and the mutex controls access to that variable. To keep the
    implementation simple, you can ignore possibilities such as init() 
    failing or being canceled when first called from a thread (i.e., it is
    not neccessary to devise a scheme whereby, if such an even occurs, the
    next thread that call one_time_init() reattempts the call to ini()).

SOLUTION

    Modify interface:

        struct once_control;
        one_time_init(struct once_control *control, void (*init)(void));

    Implement one_time_init():

        - same as hints of requirements

    Imeplement main():

        - define init_fn()
        - call one_time_init()

USAGE

    $ ./dest/bin/31-01
    thread 0x0e8ec700 call one_time_init()
    thread 0x0e8ec700 invoked init_fn()
    wait for all of threads terminate
    thread 0x0c8ec700 call one_time_init()
    thread 0x0d0ec700 call one_time_init()

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include <tlpiexer/error.h>

#define RET_SUCCESS 0
#define RET_FAILURE -1

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : function will start by thread
// arg      :
//  - data: pointer to data as params
// ret      : pointer to returned data
void* thread_entry(void *data);

// spec     : contain data to control one time call
struct once_control
{
    int called;
    pthread_mutex_t mutex;
};

// spec     : call init one time in multi thread
// arg      :
//  - control: pointer to structure contains control data
//  - ini: pointer to init function which call one time in multi thread
// ret      : 
//  - 0 on success
//  - -1 on error
int one_time_init(struct once_control *control, void (*init)(void));

// spec     : sample initialization function
// arg      : none
// ret      : none
void init_fn(void);

// spec     : global variable use for one_time_init()
struct once_control control;

// spec     : convert pthread_t to hex string
// arg      : 
//  - tid: thread identify
//  - hex: pointer to hex string
//  - size: size of hex
// ret      : 
//  - 0 on success
//  - 1 on error
int tid_to_hexstr(pthread_t tid, char *hex, int size);

int main(int argc, char **argv)
{
    // variables
    int opt;
    pthread_t thread_1, thread_2, thread_3;

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
    
    // init control
    // todo: control must init by macro on declare control
    control.called = 0;
    pthread_mutex_init(&control.mutex, NULL);

    // try create three thread
    // in each thread, call init_one_time()
    if (pthread_create(&thread_1, NULL, thread_entry, NULL) != 0)
        exit_err("pthread_create:1");
    if (pthread_create(&thread_2, NULL, thread_entry, NULL) != 0)
        exit_err("pthread_create:2");
    if (pthread_create(&thread_3, NULL, thread_entry, NULL) != 0)
        exit_err("pthread_create:3");

    // wait a moment for threads done
    // do not use pthread_join() for multi thread
    // it can drop on infinite wait
    // when main thread exited, all of thread will terminate
    printf("wait for all of threads terminate\n");
    sleep(3);

    // success
    return EXIT_SUCCESS;
}

void* thread_entry(void *data)
{
    // variables
    pthread_t tid;
    char tid_str[128];

    // init
    tid = pthread_self();
    if (tid_to_hexstr(tid, tid_str, sizeof(tid_str)) == -1) {
        fprintf(stderr, "tid_to_hexstr\n");
        return NULL;
    }

    // call one_time_init()
    printf("thread %s call one_time_init()\n", tid_str);
    if (one_time_init(&control, init_fn) == -1) {
        fprintf(stderr, "one_time_init\n");
        return NULL;
    }

    // done
    return NULL;
}

int one_time_init(struct once_control *control, void (*init)(void))
{
    // get control of control from other thread
    if (pthread_mutex_lock(&control->mutex) != 0)
        return RET_FAILURE;

    // verify called
    if (control->called)
        return RET_SUCCESS;

    // call init function
    init();

    // mark called
    control->called = 1;
    
    // back control of control to other thread
    if (pthread_mutex_unlock(&control->mutex) != 0)
        return RET_FAILURE;

    // success
    return RET_SUCCESS;
}

void init_fn(void)
{
    // variables
    pthread_t tid;
    char tid_str[128];

    // init
    tid = pthread_self();
    if (tid_to_hexstr(tid, tid_str, sizeof(tid_str)) == -1) {
        fprintf(stderr, "tid_to_hexstr\n");
        return;
    }

    // show thread invoked init_fn()
    printf("thread %s invoked init_fn()\n", tid_str);
}

int tid_to_hexstr(pthread_t tid, char *hex, int size)
{
    // variables
    unsigned char *mem = (unsigned char*)&tid;
    int i;

    // verify size of hex string to write format '0x...\0'
    if (size < sizeof(tid) + 3)
        return RET_FAILURE;

    // two first character of hex
    sprintf(hex, "0x");

    // write character to hex
    for (i = 0; i < sizeof(tid); i++) {
        sprintf(hex + 2 + i, "%02x", mem[i]);
    }

    // end character
    hex[i + 2] = '\0';

    // success
    return RET_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "31-1";

    printf("\nUSAGE:\n\n");
    printf("%s\n", exename);
    printf("%s -h\n\n", exename);
}
