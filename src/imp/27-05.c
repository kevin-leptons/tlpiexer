/*
SYNOPSIS

    Solution for exercise 27-05.

REQUIREMENT

    When we run the following program, we find it produces no output. 
    why is this?.

        #include "tlpi.hdr.h"

        int main(int argc, char *argv[])
        {
            printf("hello world");
            execlp("sleep", "sleep", "0", (char*)NULL);
        }

SOLUTION

    Because printf() put data into buffer in user space. After call execlp(),
    all of user space memory will be replaced by new program, so prev data
    won't occurs in terminal.

USAGE

    $ ./dest/bin/27-05

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("hello world");
    execlp("sleep", "sleep", "0", (char*)NULL);

    printf("New program here\n");
    return EXIT_SUCCESS;
}
