/*
SYNOPSIS

    Solution for excercise 27-04.

REQUIREMENT

    What is the effect of the following code? in what circumstances might it
    be useful?

        child_pid = fork();
        if (child_pid == -1)
            err_exit("fork_1");

        // child process perform here
        if (child_pid == 0) {
            switch(fork()) {
                case -1: 
                    // failure
                    err_exit("fork_2");
                case 0:
                    // grandchild process
                    // do real work here
                    exit(EXIT_SUCCESS);
                default:
                    // child process
                    // make grandchild an orphan
                    exit(EXIT_SUCCESS);
            }
        }

        // parent perform here
        if (waitpid(child_pid, &status, 0) == -1)
            err_exit("waitpid");

        // parent carries on to do other things

SOLUTION

    Child process exit and won't be zomebie because parent process wait for
    all of process which have process group equal it's group.

    
    After child process termiantes, grant child process will be adopt by init
    process. Then if grant child process terminates, it's result will be
    handle by init process.

USAGE

    $ ./dest/bin/27-04
    Goto ... to see solution

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    printf("Goto %s to see solution\n", __FILE__);
    return EXIT_SUCCESS;
}
