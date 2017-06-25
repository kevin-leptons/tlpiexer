/*
SYNOPSIS

    Solution for exercise 07-01.

REQUIREMENT

    Modify the program in listing 7-1 (free_and_sbrk.c) to print out the
    current value of the program break after each execution of malloc().
    Run the program specifying a small allocation block size. This will
    demontrate that malloc() doesn't employ sbrk() to adjust the program break
    on each call, but instead periodically allocates larger chucks of memory
    from which it passes back small pieces to the caller.

SOLUTION :

    Modify the program in listing 7-1 as requirements

USAGE

    $ ./dest/bin/07-01 128 4096
    break address: 0x15f8000, start allocating 4096 bytes with 128 times
    break address changed: 0x15f8000=>0x161a000
    break address changed: 0x161a000=>0x163b000
    break address changed: 0x163b000=>0x165c000
    break address changed: 0x165c000=>0x167d000
    break address: 0x167d000, free from block 1 to 128 with step 1
    break address changed: 0x167d000=>0x1619000

AUTHORS

    Michael Kerrisk 
    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MAX_MALLOCS 1000000

void showhelp();

int main(int argc, char **argv)
{
    // variables
    char *blocks[MAX_MALLOCS];
    int fstep, fmin, fmax, bsize, nallocs;
    int i;
    void *tmp_addr;
    void *brk_addr;

    // show help
    if (argc == 2 && strcmp(argv[1], "-h") == 0) {
        showhelp();
        return EXIT_SUCCESS; 
    }

    // verify arguments
    if (argc < 3) {
        showhelp();
        return EXIT_FAILURE;
    }

    nallocs = atoi(argv[1]);
    if (nallocs > MAX_MALLOCS) {
        fprintf(stderr, "num-allocs > %d\n", MAX_MALLOCS);
        return EXIT_FAILURE;
    }

    bsize = atoi(argv[2]);
    if (bsize == 0) {
        fprintf(stderr, "block-size > 0\n");
        return EXIT_FAILURE;
    }

    fstep = (argc > 3) ? atoi(argv[3]) : 1;
    fmin = (argc > 4) ? atoi(argv[4]) : 1;
    fmax = (argc > 5) ? atoi(argv[5]) : nallocs;
    
    if (fmax > nallocs) {
        fprintf(stderr, "free-max > num-allocs\n");
        return EXIT_FAILURE;
    }

    brk_addr = sbrk(0);
    printf("break address: %p, start allocating %i bytes with %i times\n", 
            brk_addr, bsize, nallocs);
    for (i = 0; i < nallocs; i ++) {
        blocks[i] = malloc(bsize);
        if (blocks[i] == NULL) {
            perror("malloc");
            return EXIT_FAILURE;
        }
        tmp_addr = sbrk(0) ;
        if (tmp_addr != brk_addr) {
            printf("break address changed: %p=>%p\n", brk_addr, tmp_addr);
            brk_addr = tmp_addr;
        }
    }

    printf("break address: %p, free from block %i to %i with step %i\n", 
            brk_addr, fmin, fmax, fstep);
    for (i = fmin - 1; i < fmax; i += fstep) {
        free(blocks[i]);
        tmp_addr = sbrk(0);
        if (tmp_addr != brk_addr) {
            printf("break address changed: %p=>%p\n", brk_addr, tmp_addr);
            brk_addr = tmp_addr;
        }
    }
    
    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "7-1";

    printf("\nUSAGE:\n\n");
    printf("\t%s num-allocs block-size [step [min [max]]]\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("ARGUMENTS:\n\n");
    printf("\tnum-allocs: number of times call malloc()\n"); 
    printf("\tblock-size: memory size invoke each time()\n"); 
    printf("\tstep: number of memory is free in each call free()\n"); 
    printf("\tmin: start of address to free\n"); 
    printf("\tmax: end of address to free\n\n"); 
}
