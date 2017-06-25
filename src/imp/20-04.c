/*
SYNOPSIS

    Solution for exercise 20-04.

REQUIREMENT

    Implement siginterrupt() using sigaction().

SOLUTION 

    Implement siginterrupt()

        - retrieve flags of signal handler by sigaction()
        - add SA_RESTART to flags
        - update flags of signal handler by sigaction()

    Implement main()

        - parse options
        - verify arguments, options
        - setup signal handler by sigaction()
        - switch to call siginterrupt()

USAGE

    NO USE siginterrupt(): read() is terminate

        Terminal 1

        $ ./dest/bin/20-04 -u n -f n
        process id: 10382
        use: kill(1) to send 'User defined signal 1' to process
        enter anything:

        sinal handler call for 'User defined signal 1'
        warning: signal User defined signal 1 interrupt, read() return -1

        Terminal 2

        $ kill -s SIGUSR1 10382

    USE siginterrupt(): read() still work

        Terminal 1
        process id: 11704
        use: kill(1) to send 'User defined signal 1' to process
        enter anything

        $ ./dest/bin/20-04

        Terminal 2
        process id: 11704
        use: kill(1) to send 'User defined signal 1' to process
        enter anything: 
        sinal handler call for 'User defined signal 1'

        # type something
        hello
        received: size=6, data=hello

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

#define RET_SUCCESS 0
#define RET_FAILURE -1
#define SIG_TEST SIGUSR1
#define BUF_SIZE 1024

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : make kernel restart i/o system call after execute signal handler 
// arg      :
//  - sig: signal number
//  - flag: 
//      - if flag = 1 and no data has been transferred, system call return -1
//        and errno = EINTR
//      - if flag = 1 and data transfer has started, system call will interrupt
//        and return actual data amount of data transferred
//      - if flag = 0, system call will be restarted if interrupted by signal
// ret      :
//  - 0 on success
//  - -1 on error
int siginterrupt(int sig, int flag);

// spec     : signal handler. this function will be call if signal has been
//            deliver to process and signal handler has setup
// arg      :
//  - sig: signal number
// ret      : none
void signal_handler(int sig);

// spec     : setup signal handler
// arg      : 
//  - sigint: 1 that mean use siginterrupt, 0 that mean not use siginterrupt
// ret      :
//  - 0 on success
//  - -1 on error
int setup_signal(int sigint, int sigint_flag);

int main(int argc, char **argv)
{
    // variables
    int opt;
    int sigint;
    int sigint_flag;
    char buf[BUF_SIZE];
    int rsize;

    // init
    sigint = -1;
    sigint_flag = -1;

    // parse options
    while ((opt = getopt(argc, argv, "hu:f:")) != -1) {
        switch (opt) {
            case 'u':
                if (strcmp(optarg, "y") == 0)
                    sigint = 1;
                else if (strcmp(optarg, "n") == 0)
                    sigint = 0;
                else {
                    showhelp();
                    return EXIT_FAILURE;
                }
                break;
            case 'f':
                if (strcmp(optarg, "y") == 0)
                    sigint_flag = 1;
                else if (strcmp(optarg, "n") == 0)
                    sigint_flag = 0;
                else {
                    showhelp();
                    return EXIT_FAILURE;
                }
                break;
            case 'h':
                showhelp();
                return EXIT_SUCCESS;
            default:
                showhelp();
                return EXIT_FAILURE;
        }
    }

    // verify argument
    // verify options
    if (optind > argc || sigint == -1 || sigint_flag == -1) {
        showhelp();
        return EXIT_FAILURE;
    }
    
    // setup signal hander
    if (setup_signal(sigint, sigint_flag) == -1)
        exit_err("setup_signal");

    // show process id
    printf("process id: %i\n", getpid());
    printf("use: kill(1) to send '%s' to process\n", strsignal(SIG_TEST));

    // use i/o system call
    // wait for input and signal deliver
    printf("enter anything: \n");
    rsize = read(STDIN_FILENO, buf, BUF_SIZE);
    if (rsize == -1 && errno == EINTR) {
        printf("warning: signal %s interrupt, read() return -1\n", 
                strsignal(SIG_TEST));
        return EXIT_SUCCESS;
    }
    if (rsize == -1) {
        perror("read");
        return EXIT_FAILURE;
    }
    buf[rsize] = '\0';
    
    // show i/o data
    printf("received: size=%i, data=%s\n", rsize, buf);
    
    // success
    return EXIT_SUCCESS;
}

int siginterrupt(int sig, int flag)
{
    // variable
    struct sigaction pre_sigact;

    // pre sigaction
    if (sigaction(SIG_TEST, NULL, &pre_sigact) == -1)
        return RET_FAILURE;

    // modify sigaction
    pre_sigact.sa_flags |= SA_RESTART;

    // update sigaction for handler
    if (sigaction(SIG_TEST, &pre_sigact, NULL) == -1)
        return RET_FAILURE;

    // success
    return RET_SUCCESS;
}

void signal_handler(int sig)
{
    // unsafe
    printf("sinal handler call for '%s'\n", strsignal(sig));
}

int setup_signal(int sigint, int sigint_flag)
{
    // variables
    struct sigaction sigact;

    // init
    sigact.sa_handler = signal_handler;
    if (sigemptyset(&sigact.sa_mask) == -1)
        return RET_FAILURE;
    sigact.sa_flags = 0;

    // add handler
    if (sigaction(SIG_TEST, &sigact, NULL) == -1)
        return RET_FAILURE;

    // use siginterrupt
    if (sigint == 1)
        if (siginterrupt(SIG_TEST, sigint_flag) == -1)
            return RET_FAILURE;

    // success
    return RET_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "20-4";

    printf("\nUSAGE:\n\n");
    printf("\t%s -u [y | n] -f [y | n]\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("OPTIONS:\n\n");
    printf("\t-u: specify call sigaction(). y = yes, n = no\n\n"); 
    printf("\t-f: specify flag which call with sigaction()\n");
    printf("\t    y = 1, n = 0\n\n"); 
}
