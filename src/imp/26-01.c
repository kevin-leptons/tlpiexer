/*
SYNOPSIS

    Solution for excercise 26-01.

REQUIREMENT

    Write a program to verify that when a child's parent termicates, a call to
    getppid() returns 1 (the process id of init).

SOLUTION

    - create child proces by call fork()
    - parent process exit imediatly
    - child process, poll parent process is no longer exist by call kill() to
      send signal zero to that
    - after kill() failure, that mean parent process is no longer exist,
      call getppid() and show result. it must be 1 

USAGE

    $ ./dest/bin/26-01
    parent process 1

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include <tlpiexer/error.h>

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

int main(int argc, char **argv)
{
    // variables
    int opt;
    pid_t pid, cid;

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

    // verify no argument has provided
    if (optind > argc) {
        showhelp();
        return EXIT_FAILURE;
    }

    // save parent process id
    pid = getpid();

    // diable stdout buffer
    setbuf(stdout, NULL);

    // create child process
    switch (cid = fork()) {
        case -1:
            exit_err("fork");
        case 0:
            // child process start from here
            
            // wait for real parent process exit
            for (;;) {
                if (kill(pid, 0) == -1) {
                    if (errno == ESRCH)
                        break;
                    else
                        _exit(1);
                }
                sleep(1);
            }

            // parent process no longer exists
            // try show parent id of child process
            printf("\nparent process %i\n", getppid());

            // success
            _exit(0);

        default:
            // parent process continue from here
            
            return EXIT_SUCCESS;
    }

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "26-1";

    printf("\nUSAGE:\n\n");
    printf("\t%s\n", exename);
    printf("\t%s -h\n\n", exename);
}
