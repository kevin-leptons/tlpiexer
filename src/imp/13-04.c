/*
SYNOPSIS

    Solution for exercise 13-04.

REQUIREMENT

    Explain why the output of the following code differs depending on whether
    standard output is redirected to a terminal or to a disk file.

        printf("If i had more time, \n");
        write(STDOUT_FILENO,
              "I would have written you a shorter letter.\n", 43);

SOLUTION

    If output to terminal, result is:

        If I have...
        I would have...

    If output to file, result is:

        I would have...
        If I have..

    Because terminal is line-buffer, printf() contain break line "\n" so
    "I I have..." will appear before "I would have...".

    File is block-buffer, printf() put data into user-buffer while write()
    put data to kernel-buffer so "I would have..." appear before "If I have.."

USAGE

    $ ./dest/bin/13-04
    If i had more time, 
    I would have written you a shorter letter.

    $ ./dest/bin/13-04 > tmp/data.txt
    $ cat tmp/data.txt
    I would have written you a shorter letter.
    If i had more time,

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    printf("If i had more time, \n");
    write(STDOUT_FILENO,
          "I would have written you a shorter letter.\n", 43);

    return EXIT_SUCCESS;
}
