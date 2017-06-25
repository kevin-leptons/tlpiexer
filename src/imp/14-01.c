/*
SYNOPSIS

    Solution for exercise 14-01.

REQUIREMENT

    Write a program that measures the time required to create and then remove
    a large number of 1-byte files form a single directory. The program should
    create file with names of the form xNNNNNN, where NNNNNN is replaced by a 
    random six-digit number. The files should be created in the reandom order
    in which their names are generated, and then deleted in increasing
    numerical order (i.e., an order that is different from that in which they
    were created). The number of files (NF) and the directory in which they
    are to be created should be specifiable on the command line. Measure the
    times required for different value of NF (e.g., in the range from 1000 to
    20,000) and for different file systems (e.g., ext2, ext3, and XFS). What
    patterns do you observe on each file system as NF increases? how do the
    various file system compare? do the results change if the files are
    creatd in increasing numberical order (x000001, x000001, x000002, and so
    on) and then deleted in the same order? If so, what do you think the
    reason(s) might be? Again, do the result vary across file-system types?.

SOLUTION

    Same as hints of requirements.

USAGE

    # create directory to put files to
    $ mkdir -p tmp

    # create files in random, delete files in increasing order
    $ time ./dest/bin/14-01 -n 20000 -d tmp/
    real    0m1.186s
    user    0m1.120s
    sys 0m0.060s

    # create files in increasing, delete files in increasing order
    $ time ./dest/bin/14-01 -n 20000 -d tmp/ -o
    real    0m0.998s
    user    0m0.716s
    sys 0m0.276s

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <tlpiexer/error.h>

#define RET_SUCCESS 0
#define RET_FAILURE -1
#define ARR_MAX 999999

// spec     : generate number into array with specify size
// arg      :
//  - arr: array where number put to
//  - size: number of element to generate
//  - order: if value is 1, number will generate from 1 to size. else, number
//    will generate randomly
// ret      :
//  - 0 on success
//  - -1 on error
int gennum(int *arr, int size, int order);

// spec     : sort array of number by increasing
// arg      :
//  - arr: array contain number to sort
//  - size: number of element
// ret      :
//  - 0 on success
//  - -1 on error
int sorti(int *arr, int size);

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : random number in [min, max]
// arg      :
//  - min: minimum value
//  - max: maximum value
// ret      : random number
int rrand(int min, int max);

int main(int argc, char **argv)
{
    // variables
    int i;
    char opt;
    int order;
    int numarr[ARR_MAX];
    int fd;
    char fpath[PATH_MAX];
    char *dpath;
    int size;
    
    // init
    size = 0;
    dpath = NULL;
    order = 0;

    // parse arguments
    while ((opt = getopt(argc, argv, "hon:d:")) != -1) {
        switch (opt) {
            case 'o':
                order = 1;
                break;
            case 'n':
                size = atol(optarg);
                break;
            case 'd':
                dpath = optarg;
                break;
            case 'h':
                showhelp();
                return EXIT_SUCCESS;
            default:
                showhelp();
                return EXIT_SUCCESS;
        }
    }

    // verify arugment
    if (size == 0 || dpath == NULL) {
        showhelp();
        return EXIT_SUCCESS;
    }

    // generate number
    if (gennum(numarr, size, order) == -1)
        exit_err("gennum");

    // create file
    // and write 1 byte to each file
    for (i = 0; i < size; i++) {
        snprintf(fpath, PATH_MAX, "%s/x%i", dpath, numarr[i]);
        fd = open(fpath, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
        if (fd == -1)
            return EXIT_SUCCESS;
        if (write(fd, "x", 1) == -1)
            return EXIT_SUCCESS; 
        if (close(fd) == -1)
            return EXIT_SUCCESS;
    } 

    // sort array by increasing
    if (sorti(numarr, size) == -1)
        return EXIT_SUCCESS;

    // delete file
    for (i = 0; i < size; i++) {
        snprintf(fpath, PATH_MAX, "%s/x%i", dpath, numarr[i]);
        if (unlink(fpath) == -1)
            return EXIT_SUCCESS;
    }

    // success
    return EXIT_SUCCESS;
}

int gennum(int *arr, int size, int order)
{
    // variables
    int i;

    // generate number depend on order
    switch (order) {
        case 0:
            for (i = 0; i < size; i++)
                arr[i] = rrand(0, ARR_MAX);
            break;
        case 1:
            for (i = 0; i < size; i++)
                arr[i] = i;
            break;
        default:
            return RET_FAILURE;
    }

    return RET_SUCCESS;
}

int sorti(int *arr, int size)
{
    // variables
    int i, j;
    int tmp;

    // no need to sort
    if (size == 1)
        return RET_SUCCESS;

    // sort
    for (i = 0; i < size - 1; i++)
        for (j = i +1; j < size; j++) {
            if (arr[i] < arr[j]) {
                tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }

    // success
    return RET_SUCCESS;
}

int rrand(int min, int max)
{
    return min + rand()%(max - min + 1);
}

void showhelp()
{
    // variables
    char exename[] = "14-1";

    printf("\nUSAGE:\n\n");
    printf("\t%s -n numfile -d dir\n", exename);
    printf("\t%s -o -n numfile -d dir\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("OPTIONS:\n\n");
    printf("\t-o: create files in increasing order\n"); 
    printf("\t-n: number of files to create then delete\n"); 
    printf("\t-d: directory where file will be create then delete\n\n"); 
}
