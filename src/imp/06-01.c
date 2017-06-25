/*
SYNOPSIS

    Solution for exercise 06-01.

REQUIREMENT

    Compile the program in listing 6-1 (mem_segments.c) and list its size
    using ls -l. Although the program contains an array (mbuf) that is 
    around 10MB in size, the executable file is much smaller than this. 
    Why is this?.

SOLUTION

    Compile program in listing 6-1

        - copy source code in listing 6-1
        - compile

    Explain

        mbuf is only declared, not initialize during compile. Memory for mbuf
        will allocate during load file into memory to run.

USAGE

    # size of binary file is smaller than sizeof(mbuf) + sizeof(gbuf)
    $ ls dest/bin/06-01
    -rwxr-xr-x 1 kevin kevin 37272 Jun 10 15:31 dest/bin/06-01

AUTHORS

    Michael Kerrisk
    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>

// uninitialized segment
char gbuf[65536];

// initialized data segment
int primes[] = {2, 3, 5, 7};

// allocated in frame for square()
static int square(int x)
{
    // allocated in frame for square()
    int result;
    result = x*x;

    // return value passed via register
    return result;
}

// allocated in frame for calc()
static void calc(int val)
{
    printf("the square of %d is %d\n", val, square(val));

    if (val < 1000) {
        // allocated in frame for calc()
        int t;

        t = val*val*val;
        printf("the cube of %d is %d\n", val, t);
    }
}

// allocated in frame for main()
int main(int argc, char **argv)
{
    // initialized data segment
    static int key = 9937;
    // uninitialized data segment
    static char mbuf[10240000];

    // allocated in frame for main()
    char *p;

    // point to memory in heap segment
    p = malloc(1024);
    printf("allocated p at %p\n", p);
    printf("mbuf at %p\n", mbuf);

    calc(key);

    exit(0);
}
