/*
SYNOPSIS

    Solution for exercise 06-02.

REQUIREMENT

    Write a program to see what happens if we try to longjump() into a
    function that has already returned.

SOLUTION

    Write program such as requirement's hint.

    Program will terminate. May be setjmp() isn't called, jumpbuf isn't
    initialize and longjmp() work with wrong pointer cause memory access
    error.

USAGE

    $ ./dest/bin/06-02
    start to jump
    Segmentation fault

AUTHOR

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>


static jmp_buf jmpbuf;

int main(int argc, char **argv)
{
    printf("start to jump\n");
    longjmp(jmpbuf, 0);

    printf("jump isn't performed\n");
    return EXIT_FAILURE;

    setjmp(jmpbuf);
    printf("jump successfully\n");
    return EXIT_SUCCESS;
}
