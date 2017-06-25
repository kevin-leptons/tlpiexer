/*
SYNOPSIS

    Solution for exercise 53-01.

REQUIREMENT

    Rewrite the programs in Listing 48-2 and Listing 48-3 (Section 48.4) as a
    threaded application, with the two threads passing data to each other via
    a global buffer, and using POSIX semaphores for synchronization.


SOLUTION

    Process contains 2 threads: thread-0 and thread-1. Thread-0 send data to
    thread-1 via buffer with semaphore synchronize.

    Thread-0 setup hander for SIGINT, SIGTERM to free resources.

    Thread-0 create and semaphore sem_r = 0, sem_w = 1.

    Thread-0 create thread-1.

    Thread-0 enter infinite loop to send data to thread-1:

        Now, sem_w = 1, sem_r = 0. Call sem_wait(sem_w). sem_w = 0, sem_r = 0.
        That mean in next loop, thread-0 will be block until sem_w = 1. sem_w
        = 1

        When thread-1 reading from buffer is done. And now, thread-1 can't read
        from buffer because thread-1 must wait for writing of thread-0.

        Read one line from stdin into buffer.

        Now, sem_w = 0, sem_r = 0. Call sem_post(sem_r). sem w = 0, sem_r = 1.
        That mean thread-1 can read from buffer.

    Thread-1 enter infinite loop to receive data from thread-0 then display it
    to stdout:

        Now, sem_w = 0, sem_r = 1. Call sem_wait(sem_r). sem_w = 0, sem_r = 0.
        That mean in next loop, thread-1 will be block until sem_r = 1. sem_r
        = 1

        When thread-0 writing to buffer is done. And now, thread-0 can't write
        to buffer because thread-0 must wait for reading of thread-1.

        Display buffer to stdout.

        Now, sem_w = 0, sem_r = 0. Call sem_post(sem_w). sem_w = 1, sem_r = 0.
        That mean thread-0 can write to buffer.

USAGE

    # run program
    $ ./dest/bin/53-01

    # type something to stdin
    # thread-0 receive data from stdin and send to thread-1
    $ hello kitty
    thread-1 received: hello kitty

    # use ctrl + v to terminate program

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define SEM_READ_NAME "/tlpiexer-sem-read-53-01"
#define SEM_WRITE_NAME "/tlpiexer-sem-write-53-01"

static char *buffer = NULL;
static sem_t *sem_w = NULL, *sem_r = NULL;
static pthread_t thread_1;

static void term_handle(int sig);

static void exit_err(const char *msg);

static void free_resouces();

void *thread_1_entry(void *argv);

int main(int argc, char *argv[])
{
    size_t rsize = sizeof(buffer);
    int flags = O_CREAT | O_EXCL | O_RDWR;
    mode_t mode = S_IRUSR | S_IWUSR;

    // setup handler for terminate signal
    if (signal(SIGINT, term_handle) == SIG_ERR)
       exit_err("thread-0: signal");
    if (signal(SIGTERM, term_handle) == SIG_ERR)
       exit_err("thread-0: signal");

    // create semaphore
    if ((sem_r = sem_open(SEM_READ_NAME, flags, mode, 0)) == NULL)
        exit_err("thread-0: sem_open read");
    if ((sem_w = sem_open(SEM_WRITE_NAME, flags, mode, 1)) == NULL)
        exit_err("thread-0: sem_open write");

    // allocate memory for buffer
    if ((buffer = malloc(BUF_SIZE)) == NULL)
        exit_err("thread-0: malloc");

    // create thread-1
    // it must wait thread-0 for writing done because sem_r = 0
    if (pthread_create(&thread_1, NULL, thread_1_entry, NULL) == -1)
        exit_err("thread-0: pthread_create");

    // enter infinite loop to send data to thread-1
    for (;;) {
        // wait buffer for write
        if (sem_wait(sem_w) == -1)
            exit_err("thread-0: sem_wait read");

        // read data from stdin into buffer
        // it mean transfer data from thread-0 to thread-1
        if (getline(&buffer, &rsize, stdin) == -1)
            exit_err("thread-0: getline");

        // unlock buffer for read
        if (sem_post(sem_r) == -1)
            exit_err("thread-0: sem_post read");
    }

    return EXIT_SUCCESS;
}

static void term_handle(int sig)
{
    pid_t pid;

    // free resources
    free_resouces();

    // reset handler to default
    // send signal again to continue handle
    signal(sig, SIG_DFL);
    pid = getpid();
    kill(pid, sig);
}

static void free_resouces()
{
    // terminate thread-1
    pthread_cancel(thread_1);
    pthread_join(thread_1, NULL);

    // free semaphore
    if (sem_r != NULL)
        sem_close(sem_r);
    if (sem_w != NULL)
        sem_close(sem_w);
    sem_unlink(SEM_READ_NAME);
    sem_unlink(SEM_WRITE_NAME);

    // free memory
    if (buffer != NULL)
        free(buffer);

    errno = 0;
}

static void exit_err(const char *caller)
{
    // free resources
    free_resouces();

    perror(caller);
    perror(strerror(errno));
    exit(EXIT_FAILURE);
}

void *thread_1_entry(void *argv)
{
    // enter infinite loop to receive data from thread-0
    // display to stdout
    for (;;) {
        // wait data available on buffer
        if (sem_wait(sem_r) == -1)
            exit_err("thread-1: sem_wait");

        // display buffer to stdout
        printf("thread-1 received: %s\n", buffer);

        // unlock buffer for write
        if (sem_post(sem_w) == -1)
            exit_err("thread-1: sem_post");
    }
}
