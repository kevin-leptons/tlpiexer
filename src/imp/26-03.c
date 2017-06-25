/*
SYNOPSIS

    Solution for exercise 26-03.

REQUIREMENT

    Replace the use of waitpid() with waitid() in ilsting 26-3
    (child_status.c).  the call to printWaitStatus() will need to be replaced
    by code that prints relevent fields from the siginfo_t structure returned
    by waitid().

SOLUTION

    - copy source code in tlpi/procexec/child_status.c
    - modify to meet requirements

USAGE

    $ ./dest/bin/26-03 -e 1
    child: pid = 6570
    waitit() returned: pid = 6570; status=0x0001 (1,0)
    child exited, status = 1

AUTHORS

    Michael Kerrisk
    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#include <tlpiexer/error.h>

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : show information from exit status
// arg      : 
//  - code: exit code
//  - status: exit status
// ret      : none
void show_siginfo(int code, int status);

int main(int argc, char **argv)
{
    // variables
    int opt;
    int child_estatus;
    int result;
    siginfo_t info;

    // init
    child_estatus = -1;

    // parse options
    while ((opt = getopt(argc, argv, "he:")) != -1) {
        switch (opt) {
            case 'e':
                child_estatus = atoi(optarg);
                break;
            case 'h':
                showhelp();
                return EXIT_SUCCESS;
            default:
                showhelp();
                return EXIT_FAILURE;
        }
    }

    // disable stdout buffer
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    // create child process
    switch (fork()) {
        case -1:
            exit_err("fork");
        case 0:
            printf("child: pid = %i\n", getpid());

            if (child_estatus != -1)
                exit(child_estatus);

            for (;;)
                pause();
        default:
            for (;;) {
                memset(&info, 0, sizeof(siginfo_t));
                result = waitid(P_ALL, 0, &info, 
                                WEXITED | WSTOPPED | WCONTINUED);
                if (result == -1)
                    exit_err("waitid");
                if (info.si_pid == 0)
                    continue;

                printf(
                    "waitit() returned: pid = %ld; status=0x%04x (%d,%d)\n", 
                    (long)info.si_pid, info.si_status,
                    (unsigned int)info.si_status, info.si_status >> 8
                );
                show_siginfo(info.si_code, info.si_status);

                if (info.si_code == CLD_EXITED || info.si_code == CLD_KILLED) 
                    exit(EXIT_SUCCESS);
            }
    }

    // success
    return EXIT_SUCCESS;
}

void show_siginfo(int code, int status)
{
    // show information
    if (code == CLD_EXITED) {
        printf("child exited, status = %d\n", status);
    } else if (code == CLD_KILLED) {
        printf("child killed by signal %d (%s)\n", status, strsignal(status));
    } else if (code == CLD_STOPPED) {
        printf("child stopped by signal %d (%s)\n", status, strsignal(status));
    } else if (code == CLD_CONTINUED) {
        printf("child continued\n");
    } else {
        printf("what happened to this child? (status=%x)\n", 
                (unsigned int)status);
    }

}

void showhelp()
{
    // variables
    char exename[] = "26-3";

    printf("\nUSAGE:\n\n");
    printf("%s [-e exit_status]\n", exename);
    printf("%s -h\n\n", exename);

    printf("OPTIONS:\n\n");
    printf("-e: status specify for argument of exit() on child process\n\n");
}
