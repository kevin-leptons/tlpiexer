/*
SYNOPSIS

    Solution for exercise 28-01.

REQUIREMENT

    Write a program to see how fast the fork() and vfork() system calls are on
    your system. Each child process should immediately exit, and the parent
    should wait() on each child before creating the nex. Compare the relative
    differences for these two system calls with those of table 28-3. The shell
    built-in command time can be used to measure the execution time of a
    program.

SOLUTION

    - create new process by n times
    - child process exit imediately
    - parent measure time use by each child
    - show result

USAGE

    $ ./dest/bin/28-01 -t 10000
    fork(): 0 secs, 750064 usecs
    vfork(): 0 secs, 108975 usecs

    $ ./dest/bin/28-01 -t 100000
    fork(): 7 secs, 713692 usecs
    vfork(): 1 secs, 190544 usecs

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <inttypes.h>

#include <tlpiexer/error.h>

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

int main(int argc, char **argv)
{
    // variables
    int opt;
    int ncreation;
    int i;
    struct timeval fork_tstart, fork_tend;
    struct timeval vfork_tstart, vfork_tend;
    ssize_t d_sec, d_usec;

    // init
    ncreation = 0;

    // parse options
    while ((opt = getopt(argc, argv, "ht:")) != -1) {
        switch (opt) {
            case 't':
                ncreation = atoi(optarg);
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
    if (optind > argc || ncreation <= 0) {
        showhelp();
        return EXIT_FAILURE;
    }

    // fork() statictist
    if (gettimeofday(&fork_tstart, NULL) == -1)
        exit_err("gettimeofday");
    for (i = 0; i < ncreation; i++) {
        switch (fork()) {
            case -1:
                exit_err("fork");
            case 0:
                // child process continue here
                
                // exit imediatly
                _exit(EXIT_SUCCESS);
            default:
                // parent process continue here
                
                // wait child process exit
                wait(NULL);

                // continue create child process
                break;
        }
    }
    if (gettimeofday(&fork_tend, NULL) == -1)
        exit_err("gettimeofday");

    // vfork() statictist
    if (gettimeofday(&vfork_tstart, NULL) == -1)
        exit_err("gettimeofday");
    for (i = 0; i < ncreation; i++) {
        switch (vfork()) {
            case -1:
                exit_err("fork");
            case 0:
                // child process continue here
                
                // exit imediatly
                _exit(EXIT_SUCCESS);
            default:
                // parent process continue here
                
                // wait child process exit
                wait(NULL);

                // continue create child process
                break;
        }
    }
    if (gettimeofday(&vfork_tend, NULL) == -1)
        exit_err("gettimeofday");

    // report
    d_sec = fork_tend.tv_sec - fork_tstart.tv_sec;
    d_usec = fork_tend.tv_usec - fork_tstart.tv_usec;
    if (d_usec < 0) {
        d_sec -= 1;
        d_usec += 1000000;
    }
    printf("fork(): %lu secs, %lu usecs\n", d_sec, d_usec);

    d_sec = vfork_tend.tv_sec - vfork_tstart.tv_sec;
    d_usec = vfork_tend.tv_usec - vfork_tstart.tv_usec;
    if (d_usec < 0) {
        d_sec -= 1;
        d_usec += 1000000;
    }
    printf("vfork(): %lu secs, %lu usecs\n",  d_sec, d_usec);

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "4-1";

    printf("\nUSAGE:\n\n");
    printf("%s -t n\n", exename);
    printf("%s -h\n\n", exename);

    printf("OPTIONS:\n\n");
    printf("-t: number of child processes will be create\n\n"); 
}
