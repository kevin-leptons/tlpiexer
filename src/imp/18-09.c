/*
SYNOPSIS

    Solution for exercise 18-09.

REQUIREMENT

    In section 18.10, we showdd different techniques (using fchdir() and
    chdir(), respectively) to return to the previous current working directory
    after changing the current working directory to another location. Suppose
    we are performing such an operation repeatedly. Which method do you expect
    to be more efficient? why? write a program to confirm your answer.

SOLUTION

    - parse options
    - verify options
    - switch to call chdir() or fchdir()

USAGE

    # create directories
    mkdir tmp/d1 tmp/d2

    # change between directories use fchdir()
    $ ./dest/bin/18-09 -u f -s tmp/d1 -t tmp/d2 -n 1000
    elapse: 0 s 636ns

    # change between directories use chdir()
    $ ./dest/bin/18-09 -u c -s tmp/d1 -t tmp/d2 -n 1000
    elapse: 0 s 2078ns

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#define _BSD_SOURCE

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <sys/time.h>
#include <fcntl.h>

#include <tlpiexer/error.h>

#define RET_SUCCESS 0
#define RET_FAILURE -1
#define USE_CHDIR 'c'
#define USE_FCHDIR 'f'

// spec     : use fchdir to change between two directory
// arg      :
//  - fpath: path to first directory
//  - spath: path to second directory
//  - n: number to change between two directory
// ret      :
//  - 0 on success
//  - -1 on error
int use_fchdir(const char *fpath, const char *spath, int n);

// spec     : use chdir to change between two directory
// arg      :
//  - fpath: path to first directory
//  - spath: path to second directory
//  - n: number to change between two directory
// ret      :
//  - 0 on success
//  - -1 on error
int use_chdir(const char *fpath, const char *spath, int n);

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

int main(int argc, char **argv)
{
    // variables
    int opt;
    char use;
    char *fpath;
    char *spath;
    int nchdir;
    struct timeval stv, ftv;

    // init
    use = 0;
    fpath = NULL;
    spath = NULL;
    nchdir = 0;

    // parse options
    while ((opt = getopt(argc, argv, "hu:s:d:n:")) != -1) {
        switch (opt) {
            case 'u':
                use = optarg[0];
                break;
            case 'd':
                fpath = optarg;
                break;
            case 's':
                spath = optarg;
                break;
            case 'n':
                nchdir = atoi(optarg);
                break;
            case 'h':
                showhelp();
                return EXIT_SUCCESS;
            default:
                showhelp();
                return EXIT_FAILURE;
        }
    }

    // varify options
    if (use == 0 || fpath == NULL || spath == NULL || nchdir == 0) {
        showhelp();
        return EXIT_FAILURE;
    }

    // change dir
    gettimeofday(&stv, NULL);
    switch (use) {
        case 'c':
            if (use_chdir(fpath, spath, nchdir) == -1) {
                perror("use_chdir");
                return EXIT_FAILURE;
            }
            break;
        case 'f':
            if (use_fchdir(fpath, spath, nchdir) == -1) {
                perror("use_fchdir");
                return EXIT_FAILURE;
            }
            break;
        default:
            showhelp();
            return EXIT_FAILURE;
    }
    gettimeofday(&ftv, NULL);

    // report time use
    printf("elapse: %li s %lins\n", 
            ftv.tv_sec - stv.tv_sec, 
            ftv.tv_usec - stv.tv_usec);

    return EXIT_SUCCESS;
}

int use_chdir(const char *fpath, const char *spath, int n)
{
    // variable
    char rfpath[PATH_MAX];
    char rspath[PATH_MAX];

    // detect absolute path
    realpath(fpath, rfpath);
    realpath(spath, rspath);

    // change directory 
    for(; n > 0; n--) {
        if (chdir(rspath) == -1)
            return EXIT_FAILURE;
        if (chdir(rfpath) == -1)
            return EXIT_FAILURE;
    }

    return RET_SUCCESS;
}

int use_fchdir(const char *fpath, const char *spath, int n)
{
    // variables
    int ffd, sfd;

    // open file
    if ((ffd = open(fpath, O_RDONLY)) == -1)
        return EXIT_FAILURE;
    if ((sfd = open(spath, O_RDONLY)) == -1)
        return EXIT_FAILURE;

    // change directory
    for(; n > 0; n--) {
        if (fchdir(sfd) == -1)
            return EXIT_FAILURE;
        if (fchdir(ffd) == -1)
            return EXIT_FAILURE;
    }

    // close
    if (close(ffd) == -1)
        return EXIT_FAILURE;
    if (close(sfd) == -1)
        return EXIT_FAILURE;
    
    return RET_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "18-9";

    printf("\nUSAGE:\n\n");
    printf("\t%s -u f -s sdir -d ddir -n num\n", exename);
    printf("\t%s -u c -s sdir -d ddir -n num\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("OPTIONS:\n\n");
    printf("\t-u: f mean that use chdir(). c mean that use fchdir()\n");
    printf("\t-s: path to first directory\n");
    printf("\t-d: path to second directory\n");
    printf("\t-n: times to change between two directory\n\n");
}
