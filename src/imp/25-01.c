/*
SYNOPSIS

    Solution for exercise 25-01.

REQUIREMENT

    If a child process makes the  call exit(-1), what exit status (as returned
    by WEXITSTATUS()) will be seen by the parent?.

SOLUTION

    -1 mean that all of left 8bits of status will be set to 1:

        11111111...

    After pass status to WEXITSTATUS(), we have unsigned value is 255.

USAGE

    $ ./dest/bin/25-01
    Child exit with WEXITSTATUS(status): 255

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#include <tlpiexer/error.h>

int main(int argc, char **argv)
{
    pid_t pid;
    int status;

    pid = fork();
    if (pid == -1)
        exit_err("fork");
    else if (pid == 0)
        exit(-1);

    wait(&status);
    if (!WIFEXITED(status)) {
        fprintf(stderr, "process doesn't exit normally\n");
        return EXIT_FAILURE;
    }
    printf("Child exit with WEXITSTATUS(status): %i\n", WEXITSTATUS(status));

    return EXIT_SUCCESS;
}
