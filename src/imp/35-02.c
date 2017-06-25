/*
SYNOPSIS

    Solution for excercise 35-02.

REQUIREMENT

    Write a set-user-id-root program that is the realtime scheduling analog
    of nice(1). The command-line interface of this program should be as
    follows.

        # ./rtsched policy priority command arg...

    In the above command, policy is r for SCHED_RR or f for SCHED_FIFO. This
    program should drop its prifileged id before execing the command, for the 
    reasons described in retrieving and modifying real, effective, and saved
    set ids and be careful when executing a program.

SOLUTION

    - parse options
    - parse arguments
    - call sched_setscheduler() to change policy and priority
    - call setresuid() to drop effective-user-id, saved-user-id
    - call execvp() to replace invoked program into current process

USAGE

    $ sudo ./dest/bin/35-02 r -1
    Password:
    asset   box.entry.sh       _config.yml  dest     install     src
    box cmake-build-debug  core     doc  license.md  tlpi
    boxctl  CMakeLists.txt     ctl      include  readme.md   tmp

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <string.h>

#include <tlpiexer/error.h>

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

int main(int argc, char *argv[])
{
    // variables
    int opt;
    int policy;
    struct sched_param sparam;
    char *cmd;
    char ** cmd_args;

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
    if (argc < 4) {
        showhelp();
        return EXIT_FAILURE;
    }

    // parse policy arguments
    if (strcmp(argv[1], "r") == 0)
        policy = SCHED_RR;
    else if (strcmp(argv[1], "f") == 0)
        policy = SCHED_FIFO;
    else {
        showhelp();
        return EXIT_FAILURE;
    }

    // parse priority argument
    sparam.sched_priority = atoi(argv[2]);

    // change schedule policy and priority
    if (sched_setscheduler(0, policy, &sparam) == -1)
        exit_err("sched_setscheduler");

    // drop privilage 
    if (setresuid(getuid(), getuid(), getuid()) == -1)
        exit_err("setresuid");

    // prepare command
    cmd = argv[3];
    cmd_args = argv + 3;

    // replace current program by invoked command
    execvp(cmd, cmd_args);

    // if process continue here, error has occur
    exit_err("execvp");
}

void showhelp()
{
    // variables
    char exename[] = "35-2";

    printf("\nUSAGE:\n\n");
    printf("\t%s policy priority command\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("ARGUMENTS:\n\n");
    printf("\tpolicy: r = SCHED_RR, f = SCHED_FIFO\n");
    printf("\tpriority: number specify priority of process\n");
    printf("\tcommand: program and arguments to execute\n\n");
}
