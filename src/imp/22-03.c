/*
SYNOPSIS

    Solution for excercise 22-03.

REQUIREMENT

    Section 22.10 stated that accepting signals using sigwaitinfo() is faster
    than the use of a signal handler plus sigsuspend(). The program
    signals/sig_speed_sigsuspend.c, suppied in the source code distribution for
    this book, uses sigsuspend() to alternately send signals back and forward
    between a parent and child process. Time the operation of this program
    to exchange on million signals between the two processes (The number of 
    signal to exchange is provided as a command-line argument to the program).
    Create a modified version of the program that instead uses sigwaitinfo(),
    and time that version. What is the speed difference between the two 
    programs?.

SOLUTION :

    - parse options
    - verify arguments, options
    - create child process. now two process running is parent and child
    - each process send/wait signal to other process. depend on options,
      call sigsuspend() or sigwaitinfo() to wait signal

USAGE

    $ time ./dest/bin/22-03 -s -n 1000
    parent process 24141
    child process 24142

    real    0m0.518s
    user    0m0.012s
    sys     0m0.280s

    $ time ./dest/bin/22-03 -s -n 1000
    parent process 24133
    child process 24134

    real    0m0.428s
    user    0m0.000s
    sys     0m0.252s

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include <tlpiexer/error.h>

#define TEST_SIG SIGUSR1

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : callback function will be call when process has received signal
// arg      :
//  - sig: signal number
// ret      : none
void sighandler(int sig);

int main(int argc, char **argv)
{
    // variables
    int opt;
    char use;
    int nsig;
    pid_t ppid, cpid;
    int i;
    sigset_t sig_waitmask;
    siginfo_t sig_info;
    struct sigaction sigact;
    sigset_t sig_blkmask;
    sigset_t sig_eptmask;

    // init
    use = 0;
    nsig = 0;
    ppid = getpid();
    sigemptyset(&sig_eptmask);

    // parse options
    while ((opt = getopt(argc, argv, "hswn:")) != -1) {
        switch(opt) {
            case 's':
                if (use == 'w') {
                    showhelp();
                    return EXIT_FAILURE;
                }
                use = 's';
                break;
            case 'w':
                if (use == 's') {
                    showhelp();
                    return EXIT_FAILURE;
                }
                use = 'w';
                break;
            case 'n':
                nsig = atoi(optarg);
                break;
            case 'h':
                showhelp();
                return EXIT_SUCCESS;
            default:
                showhelp();
                return EXIT_FAILURE;
        }
    }

    // verify options
    // verify arguments is not provided
    if (use == 0 || nsig == 0 || optind > argc || (use != 's' && use != 'w')) {
        showhelp();
        return EXIT_FAILURE;
    }

    // init
    sigemptyset(&sig_waitmask);
    sigaddset(&sig_waitmask, TEST_SIG);

    // setup signal handler
    sigact.sa_handler = sighandler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    if (sigaction(TEST_SIG, &sigact, NULL) == -1)
        exit_err("sigaction");

    // block signal before create child process
    sigemptyset(&sig_blkmask);
    sigaddset(&sig_blkmask, TEST_SIG);
    if (sigprocmask(SIG_SETMASK, &sig_blkmask, NULL) == -1)
        exit_err("sigprocmask");

    // create child process
    cpid = fork();
    switch (cpid) {
        case -1:
            exit_err("fork");
        case 0:
            // child process continue here
            
            printf("child process %i\n", getpid());
            for (i = 0; i < nsig; i++) {
                if (kill(ppid, TEST_SIG) == -1)
                    exit_err("child:kill");

                if (use == 's')
                    if (sigsuspend(&sig_eptmask) == -1 && errno != EINTR)
                        exit_err("sigsuspend");

                if (use == 'w')
                    if (sigwaitinfo(&sig_waitmask, &sig_info) == -1)
                        exit_err("sigwaitinfo");
            }
            break;
        default:
            // parent process continue here
            
            printf("parent process %i\n", getpid());
            for (i = 0; i < nsig; i++) {
                if (use == 's')
                    if(sigsuspend(&sig_eptmask) == -1 && errno != EINTR)
                        exit_err("sigsuspend");

                if (use == 'w')
                    if (sigwaitinfo(&sig_waitmask, &sig_info) == -1)
                        exit_err("sigwaitinfo");

                if (kill(cpid, TEST_SIG) == -1)
                    exit_err("parent:kill");
            }
    }

    // success
    return EXIT_SUCCESS;
}

void sighandler(int sig)
{
    // do not use i/o call, it will be make test time become not easy to 
    // compare
}
void showhelp()
{
    // variables
    char exename[] = "22-3";

    printf("\nUSAGE:\n\n");
    printf("%s [-s | -w] -n nsig\n", exename);
    printf("%s -h\n\n", exename);

    printf("OPTIONS:\n\n");
    printf("-s: use sigsuspend() to wait signal\n"); 
    printf("-w: use sigwaitinfo() to wait signal\n");
    printf("-n: number of times send signal between two process\n");
}
