/*
SYNOPSIS

    Solution for exercise 34-04.

REQUIREMENT

    Modify the program in listing 34-4 (disc_SIGHUP.c) to verify that, if the
    controlling process doesn't terminate as consequence of receiving SIGHUP,
    then the kernel doesn't send SIGHUP to the members of the foreground 
    process.

SOLUTION

    Copy source code in tlpi/pgsjc/disc_SIGHUP.c

        - parent create child processes
        - child process wait for kernel send SIGHUP
        - parent process exit normaly, not raise SIGHUP

USAGE

    # create child and change process group
    $ ./dest/bin/34-04 d
    parent pid: 12558
    foreground group id: 2245
    pid=12560 pgid=12560

    # create child and don't change process group
    $ ./dest/bin/34-04 s
    parent pid: 12573
    foreground group id: 2245
    pid=12575 pgid=2245

AUTHORS

    Michael Kerrisk
    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include <tlpiexer/error.h>

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : callback function when process received signal
// arg      :
//  - sig: signal number
// ret      : none
static void sighandler(int sig);

int main(int argc, char *argv[])
{
    // variables
    int opt;
    pid_t ppid, cpid;
    int i;
    struct sigaction sigact;

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
    if (argc <= 1) {
        showhelp();
        return EXIT_FAILURE;
    }
    for (i = 1; i < argc; i++)
        if ((strcmp(argv[i], "d") != 0) && (strcmp(argv[i], "s") != 0)) {
            showhelp();
            return EXIT_FAILURE;
        }


    // init
    // do not use buffer for stdout
    setbuf(stdout, NULL);
    ppid = getpid();

    // show information
    printf("parent pid: %ld\n", (long)ppid);
    printf("foreground group id: %i\n", tcgetpgrp(STDIN_FILENO));

    // create child processes
    for (i = 1; i < argc; i++) {
        cpid = fork();

        // fork failure
        if (cpid == -1)
            exit_err("fork");

        if (cpid == 0) {
            // child process continue here
            
            // make child process group id different than parent
            if (strcmp(argv[i], "d") == 0) {
                if (setpgid(0, 0) == -1)
                    perror("setpgid");
            } 
            
            // setup signal handler
            sigemptyset(&sigact.sa_mask);
            sigact.sa_flags = 0;
            sigact.sa_handler = sighandler;
            if (sigaction(SIGHUP, &sigact, NULL) == -1)
                exit_err("sigaction");

            // show information
            printf("pid=%ld pgid=%ld\n", (long)getpid(), (long)getpgrp());


            // wait for SIGHUP from kernel
            // when parent process exit as consequence of receving SIGHUP
            sleep(6);

            // exit
            _exit(EXIT_SUCCESS);
        }

        // parent process continue here
        // continue loop
    }

    // only parent fall to here
    
    // if process is parent
    // wait for last child setup
    // then exit
    // do not send SIGHUP to parent process by perform kill command
    // or close terminal
    sleep(3);

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "34-4";

    printf("\nUSAGE:\n\n");
    printf("%s\n", exename);
    printf("%s -h\n\n", exename);
}

static void sighandler(int sig)
{
    // unsafe
    printf("pid %ld: caught signal %2d (%s)\n", 
            (long)getpid(), sig, strsignal(sig));
}
