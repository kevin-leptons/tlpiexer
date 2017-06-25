/*
SYNOPSIS

    Solution for exercise 36-02.

REQUIREMENT

    Write a program that executes a command and then displays its resource
    usage. This is analogous to what the time(1) command does. Thus, we would
    use this program as follows.

        $ ./rusage command arg...

SOLUTION

    - parse options
    - parse arguments
    - create child process by fork(). in child process, call execvp() to 
      execute invoked program
    - parent process wait until child process exited. then call getrusage()
      to measures resources is used by child process and show them

USAGE

    $ ./dest/bin/36-02 grep -rni 36-02 src
    src/imp/36-02.c:4:    Solution for exercise 36-02.
    src/imp/36-02.c:25:    $ ./dest/bin/36-02 grep -rni 36-02 src
    Binary file src/imp/.36-02.c.swp matches
    cpu time            : 0 secs 0 usecs
    system time         : 0 secs 8000 usecs
    resident set size   : 2504 kb
    page faults non i/o : 136
    page faults i/o     : 0

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <sys/wait.h>

#include <tlpiexer/error.h>

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

int main(int argc, char *argv[])
{
    // variables
    char *cmd, **cmd_args;
    struct rusage usage;

    // parse options
    if (argc == 2 && strcmp(argv[1], "-h") == 0) {
        showhelp();
        return EXIT_SUCCESS;
    }
    // verify arguments
    if (argc < 2) {
        showhelp();
        return EXIT_FAILURE;
    }

    // create child process
    // run invoked program in child process
    switch (fork()) {
        case -1:
            // fails
            exit_err("fork");
        case 0:
            // child process continue here

            // prepare arguments to run new program
            cmd = argv[1];
            cmd_args = argv + 1;

            // execute new program
            execvp(cmd, cmd_args);

            // if process continue here, error has occur
            _exit(EXIT_FAILURE);
        default:
            // parent process continue here

            // wait for child process exited
            wait(NULL);

            // measures resources is used by child process
            // then show them
            if (getrusage(RUSAGE_CHILDREN, &usage) == -1)
                exit_err("getrusage");
            printf("%-20s: %ld secs %ld usecs\n", "cpu time",
                    usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);
            printf("%-20s: %ld secs %ld usecs\n", "system time",
                    usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);
            printf("%-20s: %ld kb\n", "resident set size", usage.ru_maxrss);
            printf("%-20s: %ld\n", "page faults non i/o", usage.ru_minflt);
            printf("%-20s: %ld\n", "page faults i/o", usage.ru_majflt);
    }

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "36-2";

    printf("\nUSAGE:\n\n");
    printf("\t%s command arg...\n", exename);
    printf("\t%s -h\n\n", exename);
}
