/*
SYNOPSIS

    Solution for excercise 24-01.

REQUIREMENT

    After a program executes the following series of fork() calls, how many
    new processes will result (assuming that none of the calls fails)?.

        fork();
        fork();
        fork();

SOLUTION

    After each fork(), number of processes is double. In simple, number of
    processes after n fork() call is 2^n - 1, except first process. 
    In this case, that is: 2^3 - 1 = 8 - 1 = 7.

    Use a pipe, new process write one byte to write-end. Parent process waits
    for 3 seconds, then read data from pipe. Number of bytes which read from
    pipe is number of new processes.

USAGE

    $ ./dest/bin/24-01
    New processes has created: 7

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <tlpiexer/error.h>

int main(int argc, char **argv)
{
    pid_t pid;
    int pfd[2];
    char buf[128];
    ssize_t rsize;
    
    pid = getpid();
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
    if (pipe(pfd) != 0)
        exit_err("pipe");

    if (fork() < 0)
        exit_err("fork:1");
    if (fork() < 0)
        exit_err("fork:2");
    if (fork() < 0)
        exit_err("fork:3");

    if (getpid() == pid) {
        sleep(3);
        rsize = read(pfd[0], buf, sizeof(buf)) ;
        if (rsize < 0)
            exit_err("read");
        printf("New processes has created: %li\n", rsize);
    }
    else {
        write(pfd[1], "x", 1);
    }

    return EXIT_SUCCESS;
}
