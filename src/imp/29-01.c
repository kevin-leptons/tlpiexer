/*
SYSOPSIS

    Solution for exercise 29-01.

REQUIREMENT

    What possible outcomes might there be if a thread executes the following
    code.

        pthread_join(pthread_self(), NULL);

    Write a program to see what actually happens on linux. If we have a
    variable, tid, containing a thread  id, how can a thread prevent itselft
    from making a call, pthread_join(tid, NULL), that is equivalent to above
    statement?.

SOLUTION :

    - call pthread_join(pthread_selft(), NULL)
    - an error will occur 

    If we have variable tid containing a thread id, we need use 
    pthread_compare(tid, pthread_self()) to verify tid is not current 
    thread id.

USAGE

    $ ./dest/bin/29-01
    Error: pthread_join

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

int main(int argc, char **argv)
{
    // variables
    int opt;

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

    // try join thread it self
    if (pthread_join(pthread_self(), NULL) != 0) {
        fprintf(stderr, "Error: pthread_join\n");
        return EXIT_FAILURE;
    }

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "29-1";

    printf("\nUSAGE:\n\n");
    printf("\t%s\n", exename);
    printf("\t%s -h\n\n", exename);
}
