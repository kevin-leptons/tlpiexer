/*
 * 
SYNOPSIS

    Solution for excercise 22-02.


REQUIREMENT

    If both a realtime and a standard signal are pending for a process, SUSv3
    leaves it unspecified which is delivered first. Write a program that shows
    what linux does in this case (Have the program set up a handler for all
    signals, block signals for a peroid time so that you can send various
    signals to it, and then unblick all signals).

SOLUTION

    - setup signal handler, to show what is happen
    - block all of signal, include standard and real time signal
    - send all of signal in random order to itself process
    - unblock all of signal

USAGE

    $ ./dest/bin/22-02
    raise 11. 'Segmentation fault'
    raise 6. 'Aborted'
    raise 4. 'Illegal instruction'
    ...

    catch 4. 'Illegal instruction'
    catch 5. 'Trace/breakpoint trap'
    catch 7. 'Bus error'
    catch 8. 'Floating point exception'
    ...

    catch missing 9. 'Killed'
    catch missing 18. 'Continued'
    catch missing 19. 'Stopped (signal)'
    catch missing 20. 'Stopped'

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

// spec     : this global variable use for signal handler
int nsig_catch = 0;
sigset_t sig_catch;

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : callback function will be call when process received signal
// arg      :
//  - sig: signal number
// ret      : none
void sighandler(int sig);

// spec     : callback function will be ba call when real time signal received
// arg      :
//  - sig: signal number
//  - info: contains information of signal
//  - ucontext: ?
// ret      : none
void rtsighandler(int sig, siginfo_t *info, void *ucontext);

// spec     : block and set handler for all of standard signal
// arg      : none
// ret      : 
//  - 0 on success
//  - -1 on success
int setup_stdsignal(void);

// spec     : block and set handler for all of real time signal
// arg      : none
// ret      :
//  - 0 on success
//  - -1 on error
int setup_rtsignal(void);

// spec     : send all of standard signal in random order to current process
// arg      : none
// ret      : 
//  - 0 on success
//  - -1 on error
int raise_stdsignal(void);

// spec     : send all of real time signal in random order to current process
// arg      : none
// ret      :
//  - 0 on success
//  - -1 on error
int raise_rtsignal(void);

// spec     : unblock all of signal
// arg      : none
// ret      :
//  - 0 on success
//  - -1 on error
int unblock_signal(void);

// spec     : random number in [min, max]
// arg      :
//  - min: minimum value
//  - max: maximum value
// ret      : random number
int rrand(int min, int max);

int main(int argc, char **argv)
{
    // variables
    int opt;
    int i;

    // init
    sigemptyset(&sig_catch);

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
    if (optind > argc) {
        showhelp();
        return EXIT_FAILURE;
    }

    // block and set handler for all of singal
    if (setup_stdsignal() == -1)
        exit_err("setup_stdsignal");
    if (setup_rtsignal() == -1)
        exit_err("setup_rtsignal");

    // send all of signal in random order to this process
    if (raise_stdsignal() == -1 )
        exit_err("raise_stdsignal");
    if (raise_rtsignal() == -1 )
        exit_err("raise_rtsignal");

    // unblock all of signal
    if (unblock_signal() == -1)
        exit_err("unblock_signal");

    printf("\ncatch %i signal\n", nsig_catch);
    for (i = 1; i <= 31; i++) {
        if (!sigismember(&sig_catch, i))
            printf("catch missing %i. '%s'\n", i, strsignal(i));
    }

    // now see what is happen
    // signal handler will be call in specify order
    // result will be show in stdout

    // success
    return EXIT_SUCCESS;
}

void sighandler(int sig)
{
    // unsafe
    printf("catch %i. '%s'\n", sig, strsignal(sig));
    nsig_catch++;
    sigaddset(&sig_catch, sig);
}

void rtsighandler(int sig, siginfo_t *info, void *ucontext)
{
    // unsafe
    printf("catch %i with value %i\n", 
            sig, info->si_value.sival_int);
}

int setup_stdsignal(void)
{
    // variables
    sigset_t sigmask;
    struct sigaction sigact;
    int i;

    // init
    sigact.sa_handler = sighandler;
    sigfillset(&sigact.sa_mask);
    sigact.sa_flags = 0;

    // block
    sigfillset(&sigmask);
    if (sigprocmask(SIG_BLOCK, &sigmask, NULL) == -1)
        return RET_FAILURE;

    // add handler
    for (i = 1; i <= 31; i++) {
        // skip for SIGKILL, SIGSTOP
        // user can not hand for that signal
        if (i == SIGKILL || i == SIGSTOP)
            continue;

        if (sigaction(i, &sigact, NULL) == -1)
            return RET_FAILURE;
    }

    // success
    return RET_SUCCESS;
}

int setup_rtsignal(void)
{
    // variables
    sigset_t sigmask;
    struct sigaction sigact;
    int i;    

    // init
    sigact.sa_sigaction = rtsighandler;
    sigfillset(&sigact.sa_mask);
    sigact.sa_flags = SA_SIGINFO;

    // block
    sigfillset(&sigmask);
    if (sigprocmask(SIG_BLOCK, &sigmask, NULL) == -1)
        return  RET_FAILURE;

    // add handler
    for (i = SIGRTMIN; i <= SIGRTMAX; i++) {
        if (sigaction(i, &sigact, NULL) == -1)
            return RET_FAILURE;
    }

    // success
    return RET_SUCCESS;
}

int raise_stdsignal(void)
{
    // variables
    sigset_t sig_done;
    int nsig_done;
    int sig;

    // do not raise SIGKILL, SIGSTOP
    // it make process can not continue
    nsig_done = 2;
    sigemptyset(&sig_done);
    sigaddset(&sig_done, SIGKILL);
    sigaddset(&sig_done, SIGSTOP);

    // raise
    // except SIGSTOP, SIGKILL
    for(; nsig_done < 31;) {
        sig = rrand(1, 31);

        // do not repeat raise
        if (sigismember(&sig_done, sig))
            continue;

        // raise and add to signal raise done
        printf("raise %i. '%s'\n", sig, strsignal(sig));
        sigaddset(&sig_done, sig);
        nsig_done++;
        raise(sig);
    }

    printf("raise %i signal done\n\n", nsig_done - 2);
    
    // success
    return RET_SUCCESS;
}

int raise_rtsignal(void)
{
    // variables
    pid_t cpid;
    int t, i;
    union sigval sigv;

    // init
    cpid = getpid();

    // raise each signal 3 times
    for (t = 0; t < 3; t++)
        for (i = SIGRTMIN; i <= SIGRTMAX; i++) {
            sigv.sival_int = rrand(0, 1024);
            if (sigqueue(cpid, i, sigv) == -1)
                return RET_FAILURE;
        }

    // success
    return RET_SUCCESS;
}

int unblock_signal(void)
{
    // variables
    sigset_t sigmask;

    // init
    sigfillset(&sigmask);

    // unblock
    if (sigprocmask(SIG_UNBLOCK, &sigmask, NULL) == -1)
        return RET_FAILURE;

    // success
    return RET_SUCCESS;
}

int rrand(int min, int max)
{
    return min + rand()%(max - min + 1);
}

void showhelp()
{
    // variables
    char exename[] = "22-2";

    printf("\nUSAGE:\n\n");
    printf("%s\n", exename);
    printf("%s -h\n\n", exename);
}

