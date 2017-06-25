/*
SYNOPSIS

    Solution for exercise 24-03.

REQUIREMENT

    Assuming that we can modify the program source code, how could we get a
    core dump of a process at a given moment in time, while letting the
    process continue execution?.

SOLUTION

    We call fork() to create child process, then call abort() to create core
    dump file.

USAGE

    # allow core dump file is created
    $ ulimit -c 10000
    $ ./dest/bin/24-03
    before create core dump file
    parent process still running

    $ ls -l core
    -rw------- 1 kevin kevin 344064 Jun 15 17:44 core

AUTHOR

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <tlpiexer/error.h>

int main(int argc, char **argv)
{
    pid_t pid;
    
    printf("before create core dump file\n");
    pid = fork();
    if (pid == -1)
        exit_err("fork");
    else if (pid == 0)
        abort();
    else
        printf("parent process still running\n");

    return EXIT_SUCCESS;
}
