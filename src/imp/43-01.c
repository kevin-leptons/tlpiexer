/*
SYNOPSIS

    Solution for exercise 43-01.

REQUIREMENT

    Write a program that measures the bandwidth provided by pipes. As 
    command-line arguments, the program should accept the number of data blocks
    to be sent and the size of each data block. After creating a pipe, the 
    program splits into two process: a child that writes the data blocks to
    the pipe as fast as possible, and a parent that reads the data blocks.
    After all daa has been read, the parent should print the elapsed time 
    required and the bandwidth (bytes transferred per second). Measure the
    bandwidth for different data block sizes.

SOLUTION

    - parse options
    - verify arguments
    - create pipe by pipe()
    - create child process by fork()
    - child process write number of each block with size  through 
      pipe by write()
    - parent process set start time, read data by read(), then set end time.
      after that, measure bandwidth by blocks*size/time
    - show bandwidth

USAGE

    # transfer 1024 bytes between pipes in 2048 times
    $ ./dest/bin/43-01 2048 1024
    transfer: blocks=2048, size=1024
    parent: start receive data
    child: start send data
    eslapsed time: 0.002791 seconds
    bandwidth: 0.699794 GB per second

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

#include <tlpiexer/error.h>

#define SEC_TO_USEC 1000000

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

int main(int argc, char *argv[])
{
    // variables
    int opt;
    long blocks;
    int size;
    int pipefd[2];
    long i;
    char *buf;
    int exit_status;
    struct timeval start, end;
    double total_time;

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

    // verify arguments
    // convert arguments 
    if (argc != 3) {
        showhelp();
        return EXIT_FAILURE;
    }
    blocks = atoi(argv[1]);
    size = atoi(argv[2]);

    // do not use buffer of stdout
    setbuf(stdout, NULL); 

    // show information
    printf("transfer: blocks=%li, size=%i\n", blocks, size);

    // prepare buffer
    if ((buf = malloc(size)) == NULL)
        exit_err("malloc");

    // create pipe
    if (pipe(pipefd) == -1)
        exit_err("pipe");

    // create child process
    switch (fork()) {
        case -1:
            exit_err("fork");
        case 0:
            // child process continue here
            
            // close read fd of pipe to avoid error
            if (close(pipefd[0]) == -1)
                exit_err("pipefd");

            // send data to parent process
            printf("child: start send data\n");
            for (i = 0; i < blocks; i++)
                if (write(pipefd[1], buf, size) == -1)
                    exit_err("write");

            // free resource
            if (close(pipefd[1]) == -1)
                exit_err("close");

            // fall to next statement
            break;
        default:
            // parent process continue here
            
            // close write fd of pipe to avoid error
            if (close(pipefd[1]) == -1)
                exit_err("close");

            // set start time
            if (gettimeofday(&start, NULL) == -1)
                exit_err("gettimeofday");
            
            // receive data from child process
            printf("parent: start receive data\n");
            errno = 0;
            for (;;)
                if (read(pipefd[0], buf, size) <= 0)
                    break;
            if (errno != 0)
                exit_err("read");

            // set end time
            if (gettimeofday(&end, NULL) == -1)
                exit_err("gettimeofday");

            // wait for child process exit
            if (wait(&exit_status) == -1)
                exit_err("wait");

            // verify that child process has no error occur
            if (!WIFEXITED(exit_status))
                fprintf(stderr, "child process has terminate\n");
            if (WEXITSTATUS(exit_status) != 0)
                fprintf(stderr, "child process exit with code %i\n", 
                        WEXITSTATUS(exit_status));

            // calculate bandwidth
            total_time = end.tv_sec - start.tv_sec +
                (end.tv_usec - start.tv_usec) / (float) SEC_TO_USEC;
            printf("eslapsed time: %f seconds\n", total_time);
            printf("bandwidth: %f GB per second\n", 
                    (blocks * size) / total_time / 1024 / 1024 / 1024);
            
            // release resource
            if (close(pipefd[0]) == -1)
                exit_err("close");

            // fall to next statements
            break;
    }

    // free resource
    free(buf);

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "43-1";

    printf("\nUSAGE:\n\n");
    printf("\t%s blocks size\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("ARGUMENTS:\n\n");
    printf("\tblocks: number of block to transfer\n");
    printf("\tsize: size of each block\n\n");
}
