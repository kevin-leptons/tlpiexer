/*
SYNOPSIS

    Solution for exercise 34-03.

REQUIREMENT

    Write a program to verify that a cal to setsid() from process group
    leader fails.

SOLUTION

    When start any program from shell, it become process group leader. So we
    can use this program to call setsid() as a process group leader.

    Implement main():

        - parse options
        - verify arguemnts, options
        - verify process is not session leader
        - verify process is group leader
        - perform setsid(). it must raise error

USAGE

    $ ./dest/bin/34-03
    verif process is not process session leader: yes
    verify process is process group leader: no

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <tlpiexer/error.h>

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

int main(int argc, char *argv[])
{
    // variables
    int opt;
    pid_t gid, pid, sid;
    
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

    // verify no arguments has provided
    if (optind > argc) {
        showhelp();
        return EXIT_FAILURE;
    }

    // init
    pid = getpid();
    gid = getpgrp();
    if ((sid = getsid(pid)) == -1)
        exit_err("getsid");

    // verify process is not session leader
    // it true if process id is not equal session id
    if (pid == sid) {
        printf("verif process is not process session leader: no\n");
        return EXIT_FAILURE;
    }
    printf("verif process is not process session leader: yes\n");

    // verify process is group leader
    // it true if process id equal process group id
    if (pid != gid) {
        printf("verify process is process group leader: no\n");
        return EXIT_FAILURE;
    }
    printf("verify process is process group leader: yes\n");

    // perform setsid()
    // it must failure
    if (setsid() == -1)
        exit_err("setsid");

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "34-3";

    printf("\nUSAGE:\n\n");
    printf("\t%s\n", exename);
    printf("\t%s -h\n\n", exename);
}
