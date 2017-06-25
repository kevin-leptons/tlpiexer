/*
SYNOPSIS

    Solution for exercise 18-07.

REQUIREMENT

    Write a program that uses nftw() to traverse a directory tree and finishes
    by printing out conts and percentages of the various types 
    Wregular, directory, symbolic link, and so on) of file in the tree.

SOLUTION

    - define walkthough handler
    - call nftw() to walkthough directory and calculate percentages of 
      file type
    - show result

USAGE

    # walk though directory and calculate percentages of file type.
    # if dir is not specific, default is current directory
    $ ./dest/bin/18-07 .
    regular         : 90.73%
    dir             : 8.83%
    link            : 0.44%

AUTHOR

    Kevin Lepotns <kevin.leptons@gmail.com>
*/

#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ftw.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#include <tlpiexer/error.h>

#define RET_SUCCESS 0
#define RET_FAILURE -1

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : callback for each entry in directory
static int
entrycb(const char *path, const struct stat *status, int type,
        struct FTW *ftw);

// file type statictist
int nreg;
int ndir;
int nlink;

int main(int argc, char **argv)
{
    // variables
    char opt;
    int flags;
    char dpath[PATH_MAX];
    float total;

    // init
    flags = FTW_PHYS;
    nreg = 0;
    ndir = 0;
    nlink = 0;

    // parse options
    while ((opt = getopt(argc, argv, "h")) != -1) {
        switch (opt) {
            case 'h':
                showhelp();
                return EXIT_SUCCESS;
            default:
                showhelp();
                return EXIT_FAILURE;
        }
    }

    // path to directory
    if (optind >= argc)
        strcpy(dpath, ".");
    else
        strcpy(dpath, argv[optind]);

    // nftw
    if (nftw(dpath, entrycb, 8, flags) == -1)
        exit_err("nftw");

    // show
    total = nreg + ndir + nlink; 
    printf("%-16s: %3.2f%%\n", "regular", nreg/total*100);
    printf("%-16s: %3.2f%%\n", "dir", ndir/total*100);
    printf("%-16s: %3.2f%%\n", "link", nlink/total*100);
    
    // success
    return EXIT_SUCCESS;
}

static int
entrycb(const char *path, const struct stat *status, int type, struct FTW *ftw)
{
    // variables

    if (S_ISREG(status->st_mode))
        nreg++;
    else if (S_ISLNK(status->st_mode))
        nlink++;
    else if (S_ISDIR(status->st_mode))
        ndir++;
    
    // success
    return RET_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "18-7";

    printf("\nUSAGE:\n\n");
    printf("\t%s [dir]\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("ARGUMENTS:\n\n");
    printf("\tdir: path to directory\n\n"); 
}
