/*
SYNOPSIS

    Solution for exercise 35-01.

REQUIREMENT

    Implement the nice(1) command.

SOLUTION

    - detect current niceness value by call getpriority()
    - verify arguments, if match case '<cmd>', print current niceness,
      then exit
    - parse options
    - detect niceness value
    - perform setpriority() to change niceness value of current process.
      so after replace this program by command, it will be have correct
      niceness value
    - prepare arguments for execvp()
    - perform execvp() to replace this program by program is specify by 
      command arguments

USAGE

    # show current niceness value, alway is 0
    $ ./dest/bin/35-01

    # execute command with niceness 10
    $ ./dest/bin/35-01 ls
    asset   box.entry.sh       _config.yml  dest     install     src
    box cmake-build-debug  core     doc  license.md  tlpi
    boxctl  CMakeLists.txt     ctl      include  readme.md   tmp

    # execute command with niceness value equal current niceness + n. 
    $ ./dest/bin -n 3 ls
    asset   box.entry.sh       _config.yml  dest     install     src
    box cmake-build-debug  core     doc  license.md  tlpi
    boxctl  CMakeLists.txt     ctl      include  readme.md   tmp

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/resource.h>

#include <tlpiexer/error.h>

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

int main(int argc, char *argv[])
{
    // variables
    int opt;
    int cnice, nice;
    char *cmd;
    char **cmd_args;

    // init
    nice = 10;
    cmd = NULL;
    cmd_args = NULL;

    // retrieve current niceness value
    errno = 0;
    cnice = getpriority(PRIO_PROCESS, 0);
    if (errno != 0)
        exit_err("getpriority");

    // show current niceness value
    // then exit
    if (argc == 1) {
        printf("%i\n", cnice);
        return EXIT_SUCCESS;
    }

    // parse options
    while ((opt = getopt(argc, argv, "hn:")) != -1) {
        switch (opt) {
            case 'n':
                nice = atoi(optarg);
                break;
            case 'h':
                showhelp();
                return EXIT_SUCCESS;
            default:
                showhelp();
                return EXIT_FAILURE;
        }
    }

    // parse argument
    if (optind >= argc) {
        showhelp();
        return EXIT_FAILURE;
    } 
    
    // prepare command string
    cmd = argv[optind];
    cmd_args = argv + optind;

    // determine niceness value
    nice += cnice;

    // change niceness of current process
    // so after call exec(), new program will have extract niceness value
    if (setpriority(PRIO_PROCESS, getpid(), nice) == -1)
        exit_err("setpriority");

    // perform exec() to start program
    execvp(cmd, cmd_args);

    // if process continue here, error has occur
    exit_err("execvp");
}

void showhelp()
{
    // variables
    char exename[] = "4-1";

    printf("\nUSAGE:\n\n");
    printf("\t%s\n", exename);
    printf("\t%s command\n", exename);
    printf("\t%s -n value command\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("ARGUMENTS\n\n");
    printf("\tcommand: command to execute\n\n");

    printf("OPTIONS:\n\n");
    printf("\t-n: increasing niceness value\n\n");
}
