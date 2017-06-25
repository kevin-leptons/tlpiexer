/*
SYNOPSIS

    Solution for exercise 13-03.

REQUIREMENT

    What is the effect of the following statements?

        fflush(fp);
        fsync(fileno(fp));

SOLUTION

    If data in buffer is small enough, fflush() put data from user space to
    kernel space and kernel still hold that data in buffer. Then fsync() put
    data from kernel buffer to device. Everything is normal here.

    This program below try to write 1 bytes data to file stream then performs
    same as above statements.

USAGE

    $ ./dest/bin13-03

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <tlpiexer/error.h>

int main(int argc, char **argv)
{
    FILE *f;

    if (argc != 2) {
        fprintf(stderr, "Use: %s file\n", argv[0]);
        return EXIT_FAILURE;
    }

    f = fopen(argv[1], "w");
    if (f == NULL)
        exit_err("fopen");

    if (fwrite("a", 1, 1, f) != 1)
       exit_err("fwrite") ;

    fflush(f);
    fsync(fileno(f));

    fclose(f);
    return EXIT_SUCCESS;
}
