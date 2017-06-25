/*
SYNOPSIS

    Solution for exercise 34-07.

REQUIREMENT

    Write a program to verify that if one of the signals SIGTTIN, SIGTTOU,
    or SIGTSTP is sentto a member of an orphaned process group, then the 
    signal is discared (i.e, has no effect) if it would to stop the process
    (i.e., the disposition is SIG_DFL), but is delivered if a handler is 
    installed for the signal.

SOLUTION :

    - create two process in orphaned process group. that can be done by create
      child process, set it to new process group. then create two grandchild
      process, and child process exit imediately. two grandchild process will
      become processes in orphaned process group

    - first grandchild process, set handler for SIGTTIN, SIGTTOU, SIGTSTP 
      to SIG_DFL. then call raise() to send SIGTTIN, SIGTTOU, SIGSTP to itself.
      process must continue run. try write some thing to stdout to verify

    - second grandchild process, set custom handler for SIGTTIN, SIGTTOU, 
      SIGTSTP. handler must write some thing to stdout to verify. 
      then call raise() to send SIGTTIN, SIGTTOU, SIGSTP to itself. handler
      must be called

USAGE

    $ ./dest/bin/34-07
    wait for moment, message will be come
    grandchild_2
    grandchild_1
    $ grandchild_1: process continue here
    catch 21. Stopped (tty input)
    catch 22. Stopped (tty output)
    catch 20. Stopped

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

#include <tlpiexer/error.h>

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : fucntion will call when signale has received
// arg      :
//  - sig: signal number
// ret      : none
void sighandler(int sig);

int main(int argc, char *argv[])
{
    // variables
    int opt;
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

    // verify no arguments has provided
    if (optind > argc) {
        showhelp();
        return EXIT_FAILURE;
    }

    // do not use for stdout
    setbuf(stdout, NULL);

    // init
    printf("wait for moment, message will be come\n");

    // create two grandchild process which in orphaned process group
    switch (fork()) {
        case -1:
            // fails
            exit_err("fork");
        case 0:
            // child process continue here
            
            // set itself to new process group
            if (setpgid(0, 0) == -1)
                exit_err("setpgid");

            // create first grandchild
            switch (fork()) {
                case -1:
                    // fails
                    exit_err("fork");
                case 0:
                    // first grandchild continue here
                    printf("grandchild_1\n");
                    
                    // wait for child process exited
                    sleep(3);

                    // in default, signal handler for SIGTTIN, SIGTTOU, 
                    // SIGTSTP refer to SIG_DFL
                    
                    // raise signal
                    raise(SIGTTIN);
                    raise(SIGTTOU);
                    raise(SIGTSTP);

                    // it must fall here
                    // because the above signal is discarded
                    printf("grandchild_1: process continue here\n");

                    // success
                    _exit(EXIT_SUCCESS);
                default:
                    // child process continue here
                    printf("grandchild_2\n");
                    
                    // fall to next statement
                    break;
            }

            // create second grandchild
            switch (fork()) {
                case -1:
                    // fails
                    exit_err("fork");
                case 0:
                    // second grandchild continue here
                    
                    // wait for child process exited
                    sleep(3);

                    // init
                    sigact.sa_handler = sighandler;
                    sigact.sa_flags = 0;
                    sigemptyset(&sigact.sa_mask);

                    // setup handler for SIGTTIN, SIGTTOU, SIGTSTP
                    if (sigaction(SIGTTIN, &sigact, NULL) == -1)
                        exit_err("sigaction");
                    if (sigaction(SIGTTOU, &sigact, NULL) == -1)
                        exit_err("sigaction");
                    if (sigaction(SIGTSTP, &sigact, NULL) == -1)
                        exit_err("sigaction");

                    // raise signal
                    // it will be catch by signal handler
                    raise(SIGTTIN);
                    raise(SIGTTOU);
                    raise(SIGTSTP);

                    // success
                    _exit(EXIT_SUCCESS);
                default:
                    // child process continue here
                    
                    // fall to next statement
                    break;
            }

            // exit to make two grandchild become process orphaned
            // process group
            _exit(EXIT_SUCCESS);
            
        default:
            // parent process continue here
            
            // fall to next statement
            break;
    }

    // wait for all of childs exited
    wait(NULL);

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "34-7";

    printf("\nUSAGE:\n\n");
    printf("%s\n", exename);
    printf("%s -h\n\n", exename);
}

void sighandler(int sig)
{
    // unsafe
    printf("catch %i. %s\n", sig, strsignal(sig));
}
