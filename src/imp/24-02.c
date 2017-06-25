/*
SYNOPSIS

    Solution for exercise 24-02.

REQUIREMENT

    Write a program to demonstrate that after a vfork(), the child process can
    close a file descriptor (e.g., descriptor 0) without affecting the
    corresponding file descriptor in the parent.

SOLUTION

    - call vfork() to create new process
    - in new process, close STDOUT_FILENO then call _exit() to terminate
    - parent process call wait() to wait child process terminate then
      write something via STDOUT_FILENO

    If terminal display what is parent process write via STDOUT_FILENO, that
    mean child process close file descriptor will not affect to parent process.

USAGE

    $ ./dest/bin/24-02
    child process: 13205
    child: close STDOUT_FILENO
    parent process: 13204
    parent: use STDOUT_FILENO

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

#include <tlpiexer/error.h>

#define SIG_DONE SIGRTMIN

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

int main(int argc, char **argv)
{
    // variables
    int opt;
    pid_t cpid;
    char stdout_msg[] = "parent: use STDOUT_FILENO\n";

    // parse options
    while ((opt = getopt(argc, argv, "h")) != -1) {
        switch (opt) {
            case 'h':
                showhelp();
                return EXIT_SUCCESS;
            default:
                showhelp();
                EXIT_FAILURE;
        }
    }

    // verify no argument has provided
    if (optind > argc) {
        showhelp();
        return EXIT_FAILURE;
    }

    // create child process
    switch (cpid = vfork()) {
        case -1:
            exit_err("vfork");
        case 0:
            // child process
            printf("child process: %i\n", getpid());
            
            // close stdout file description
            printf("child: close STDOUT_FILENO\n");
            if (close(STDOUT_FILENO) == -1)
                return EXIT_FAILURE;

            // must use _exit() instead of break or return
            _exit(0);
        default:
            // parent process
            printf("parent process: %i\n", getpid());

            // wait for child process exit
            wait(NULL); 

            // try use STDOUT_FILENO
            if (write(STDOUT_FILENO, stdout_msg, sizeof(stdout_msg)) == -1)
                exit_err("write:STDOUT_FILENO");
    }

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "24-2";

    printf("\nUSAGE:\n\n");
    printf("%s\n", exename);
    printf("%s -h\n\n", exename);
}
