/*
SYNOPSIS

    Solution for exercise 24-04.

REQUIREMENT

    Experiment with the program in listing 24-5 (fork_whos_on_first.c) on
    other unix implementations to determine how these implementations
    schedule the parent and child processes after after a fork().

SOLUTION

    - copy source code in tlpi/procexec/fork_whos_on_first.c

USAGE

    $ ./dest/bin/24-04
    parent 0
    child 0

    parent 1
    child 1

    parent 2
    child 2

    parent 3
    child 3

    parent 4
    child 4

AUTHORS

    Michael Kerrisk
    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

#include <tlpiexer/error.h>

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

int main(int argc, char **argv)
{
    // variables
    int opt;
    int nchild, i;
    pid_t cpid;

    // init
    nchild = -1;

    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    // parse argument
    while ((opt = getopt(argc, argv, "hn:")) != -1) {
        switch (opt) {
            case 'n':
                nchild = atoi(optarg);
                break;
            case 'h':
                showhelp();
                return EXIT_SUCCESS;
            default:
                showhelp();
                return EXIT_FAILURE;
        }
    }

    // verify no argument has provided
    // verify options
    if (optind > argc || nchild == -1) {
        showhelp();
        return EXIT_FAILURE;
    }

    // make stdout unbuffered
    setbuf(stdout, NULL);

    // create number of child process
    for (i = 0; i < nchild; i++) {

        // use enter line for more easy to see
        printf("\n");

        switch (cpid = fork()) {
            case -1:
                exit_err("fork");
            case 0:
                printf("child %i\n", i);
                _exit(EXIT_SUCCESS);
            default:
                printf("parent %i\n", i);
                wait(NULL);
        }
    }

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "24-4";

    printf("\nUSAGE:\n\n");
    printf("\t%s -n nchild\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("OPTIONS:\n\n");
    printf("\t-n: number of child process will create\n\n");
}
