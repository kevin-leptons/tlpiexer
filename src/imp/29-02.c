/*
SYNOPSIS

    Solution for excercise 29-02.

REQUIREMENT

    Aside from the absence of error checking and various variable and
    structure declarations, what is the problem with the following program?.

    static void *thread_func(void *arg)
    {
        struct some_struct *pbuf = (struct some_struct*)arg;

        // do some work with structure pointed to by 'pbuf'
    }

    int main(int argc, char *argv[])
    {
        struct some_struct buf;

        pthread_create(&thr, NULL, thread_func, (void*)&buf);
        pthread_exit(NULL);
    }

SOLUTION

    After main thread exit, sub thread run continue use memory of process.

USAGE

    $ ./dest/bin/29-02

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    printf("Goto %s to see solution\n", __FILE__);
    return EXIT_SUCCESS;
}
