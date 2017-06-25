/*
SYNOPSIS

    Solution for exercise 53-02.

REQUIREMENT

    Modify the program in Listing 53-3 (psem_wait.c) to use sem_timedwait()
    instead of sem_wait(). The program should take an addition command-line
    argument that specifies a (relative) number of seconds to be used as the
    timeout for the sem_timedwait() call.

SOLUTION

    Copy source in "tlpi/psem/psem_wait.c" to "tlpi/psem/psem_wait_52-3.c".
    Then add program into "tlpi/psem/Makefile".

    Parse command line argument "-t" and convert it to integer. That value
    mean relative time in seconds which program wait until semaphore doesn't
    greater than one.

    Replace "sem_wait()" with "sem_timedwait()".

    Free semaphore.

USAGE

    NON TIMEOUT

    # change working directory to posix directory
    $ cd tlpi/psem

    # create shemaphore
    $ ./psem_create -cx /tlpiexer-53-02

    # wait semaphore above in background with timeout=100s
    $ ./psem_wait_53-02 -t 100 /tlpiexer-53-02 &

    # increase semaphore above
    # psem_wait_53-2 will unlock and print information of semaphore
    $ ./psem_post /tlpiexer-53-02
    <PID> sem_timedwait() succeeded

    TIMEOUT

    # wait semaphore above with timeout=3s
    # don't increase that semaphore
    # 3s later, error will be print
    ./psem_wait_53-2 -t 3 /tlpiexer-53-02
    ERROR [ETIMEDOUT Connection timed out] sem_timedwait

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("Goto %s to see solution\n", __FILE__);
    return EXIT_SUCCESS;
}
