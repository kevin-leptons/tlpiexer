/*
SYNOPSIS

    Solution for excercise 26-02.

REQUIREMENT

    Suppose that we have three process related as grandparent, parent, and
    child, and that the grandparent doesn't immediately perform a wait() after
    the parent exits, so that the parent becomes a zombie. When do you expect
    the grandchild to be adopted by init (so that getppid() in the grandchild
    return 1): after the parent terminates or after the grandparent does a
    wait()? write a program to verify your answer.

SOLUTION

    - create child process
    - child process create grandchild process then call _exit() to exit
      imediately
    - grandchild: wait for child process become zomebie by call sleep()
    - grandchild: show parent process id, it must be 1
    - grandchild: send SIGRTMAX to parent by call kill() to inform parent
      process perform wait() to remove child process zombie
    - grandchild: wait for child zombie disapear
    - grandchild: show parent process id, it must be 1

    It mean that grandchild will become child of init process after child
    exit, regardless parent perform wait() or not.

USAGE

    $ ./dest/bin/26-02
    parent:wait SIGRTMAX
    grandchild: before parent call wait(): getppid() = 1
    grandchild:kill SIGRTMAX to 10481
    parent: call wait() done
    $ grandchild: after parent call wait(): getppid() = 1

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include <tlpiexer/error.h>

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : function will call when create child process done
//            function will create grandchild process
// arg      : 
//  - rid: parent process id
// ret      :
//  - 0 on success
//  - -1 on error
int child_entry(pid_t rid);

int main(int argc, char **argv)
{
    // variables
    int opt;
    pid_t pid, cid;
    sigset_t sigmask;

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
    if (optind > argc ) {
        showhelp();
        return EXIT_FAILURE;
    }

    // save parent process id
    pid = getpid();

    // disable stdout buffer
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    // block SIGRTMAX before create child process
    // so during create, SIGRTMAX will not lose
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGRTMAX);
    if (sigprocmask(SIG_BLOCK, &sigmask, NULL) == -1)
        exit_err("sigprocmask");

    // create child process
    switch (cid = fork()) {
        case -1:
            return EXIT_FAILURE;
        case 0:
            // child process continue here
            
            //  use child entry for easy to read
            exit(child_entry(pid));
        default:
            // parent process continue here
            
            // child process will create grandchild process
            // then exit to become zombie
            // wait SIGRTMAX from grandchild to use wait()
            printf("parent:wait SIGRTMAX\n");
            if (sigwaitinfo(&sigmask, NULL) == -1)
                exit_err("sigwaitinfo");

            // use wait() to remove child zombie
            wait(NULL);
            printf("parent: call wait() done\n");
    }

    // success
    return EXIT_SUCCESS;
}

int child_entry(pid_t rid)
{
    // variable
    pid_t cpid;

    // create grandchild process
    switch (cpid = fork()) {
        case -1:
            exit(EXIT_FAILURE);
        case 0:
            // grandchild process continue here
            
            // wait for child process become zomebie
            sleep(3);

            // show parent of grandchild process now
            printf("grandchild: before parent call wait(): getppid() = %i\n", 
                    getppid());

            // send SIGRTMAX to inform parent process call wait()
            // so zomebie of child process will disapear
            printf("grandchild:kill SIGRTMAX to %i\n", rid);
            if (kill(rid, SIGRTMAX) == -1)
                exit_err("grandchild:kill");

            // wait parent for called wait() successfuly
            // show parent of grandchild process now
            sleep(3);
            printf("grandchild: after parent call wait(): getppid() = %i\n",
                    getppid());

            break;
        default:
            // child process continue here
            
            // exit to become zomebie
            // because parent process not call wait()
            break;
    }

    // success
    exit(EXIT_SUCCESS);
}

void showhelp()
{
    // variables
    char exename[] = "26-2";

    printf("\nUSAGE:\n\n");
    printf("%s\n", exename);
    printf("%s -h\n\n", exename);
}
