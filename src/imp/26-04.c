/*
SYNOPSIS

    Solution for excercise 26-04.

REQUIREMENT

    Listing 26-4 (make_zombie.c) uses a call to sleep() to allow the child
    process a chance to execute and terminate before the parent executes
    system(). This approach produces a theoretical reace condition. Modify
    the program to eliminate the race condition by using signals to 
    synchronize the parent and child.

SOLUTION

    - copy source code in tlpi/procexe/make_zomebie.c
    - modify to meet requirements

USAGE

    $ ./dest/bin/26-04 
    parent: pid=10386
    child: pid=10388
    10386 pts/0    00:00:00 26-04
    10388 pts/0    00:00:00 26-04 <defunct>
    after sending SIGKILL to zombie pid=10388
    10386 pts/0    00:00:00 26-04
    10388 pts/0    00:00:00 26-04 <defunct>
    after calling wait(): 10388
    10386 pts/0    00:00:00 26-04

AUTHORS

    Michael Kerrisk
    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/wait.h>

#include <tlpiexer/error.h>

#define CMD_SIZE 200

int main(int argc, char **argv)
{
    // variables
    char cmd[CMD_SIZE];
    pid_t cid;
    sigset_t sigmask;
    int sig;

    // disable stdout buffer
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    // show parent process
    printf("parent: pid=%ld\n", (long)getpid());

    // block SIGCHLD, so during fork, SIGCHLD is not lost
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGCHLD);
    if (sigprocmask(SIG_BLOCK, &sigmask, NULL) == -1)
        exit_err("sigprocmask");

    // create child process, parent do no use wait()
    // then child process exit to become zombie
    switch (cid = fork()) {
        case -1:
            exit_err("fork");
        case 0:
            // child process continue here
            
            printf("child: pid=%ld\n", (long)getpid());
            _exit(EXIT_SUCCESS);
        default:
            // parent process continue here
            
            // wait for child exit by wait until receive SIGCHLD
            if (sigwait(&sigmask, &sig) == -1)
                exit_err("sigwait");
            
            // invoke shell command to show zombie process
            snprintf(cmd, CMD_SIZE, "ps | grep %s", basename(argv[0]));
            cmd[CMD_SIZE - 1] = '\0';
            system(cmd);

            // try send SIGKILL to child zombie
            if (kill(cid, SIGKILL) == -1)
                exit_err("kill");

            // wait for child zomebie change
            sleep(3);
            
            // invoke shell command to show zombie process again
            printf("after sending SIGKILL to zombie pid=%ld\n", (long)cid);
            system(cmd);

            // use wait() to invoke kernel remove child zombie
            wait(NULL);

            // wait for child zomebie change
            sleep(3);

            // invoke shell command to show zombie process again
            printf("after calling wait(): %li\n", (long)cid);
            system(cmd);
    }
    

    // success
    return EXIT_SUCCESS;
}
