/*
SYNOPSIS

    Solution for excersice 18-06.

REQUIREMENT

    Modify the program in listing 18-3 (ntfw_dir_tree.c) to use the FTW_DEPTH
    flag. Not the difference in the order in which directory tree is traversed.

SOLUTION

    - copy source code in tlpi/dirs_links/nftw_dir_tree.c
    - modify to meet requirements

USAGE

    $ ./dest/bin/18-06 tmp
    -      l3.txt
    -      l4.txt
    -      f.txt
    -      l2.txt
    -      l1.txt
    -          prog
    -          file
    d          dir
    d      ax
    d   tmp

AUTHOR
    
    Kevin Leptons <kevin.leptons@gmail.com>
*/

#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ftw.h>
#include <string.h>
#include <unistd.h>

#include <tlpiexer/error.h>

#define RET_SUCCESS 0
#define RET_FAILURE -1

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : callback function when use nftw
// arg      :
//  - path: path to file
//  - status: status of file
//  - ftw: ?
// rete     : none
static int 
dirtree(const char *path, const struct stat *status, int type,
        struct FTW *ftw);

int main(int argc, char **argv)
{
    // variables
    int flags;
    char opt;
    char *dpath;

    // init
    flags = 0;
    flags |= FTW_DEPTH;

    // parse options
    while ((opt = getopt(argc, argv, "hmp")) != -1) {
        switch (opt) {
            case 'm': 
                flags |= FTW_MOUNT; 
                break;
            case 'p': 
                flags |= FTW_PHYS; 
                break;
            case 'h':
                showhelp();
                return EXIT_SUCCESS;
            default: 
                showhelp();
                return EXIT_FAILURE;
        }
    }
    
    // verify options
    if (argc > optind + 1) {
        showhelp();
        return EXIT_FAILURE;
    }

    // use path or current directory
    dpath = argc > optind ? argv[optind] : ".";

    // nftw
    if (nftw(dpath, dirtree, 10, flags) == -1)
        exit_err("nftw");

    // success
    return EXIT_SUCCESS;
}

static int 
dirtree(const char *path, const struct stat *status, int type, struct FTW *ftw)
{
    // variables
    char fmd[8];

    // init
    fmd[0]= '\0';

    // file type
    switch (status->st_mode & S_IFMT) {
        case S_IFREG: 
            strcat(fmd, "-");
            break;
        case S_IFDIR:
            strcat(fmd, "d");
            break;
        case S_IFCHR:
            strcat(fmd, "c");
            break;
        case S_IFBLK:
            strcat(fmd, "b");
            break;
        case S_IFLNK:
            strcat(fmd, "l");
            break;
        case S_IFIFO:
            strcat(fmd, "p");
            break;
        case S_IFSOCK:
            strcat(fmd, "s");
            break;
        default:
            strcat(fmd, "?");
    }

    // inode

    // show
    printf("%s %*s %s\n", fmd, 4*ftw->level, " ",  path + ftw->base);

    // success
    return RET_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "18-6";

    printf("\nUSAGE:\n\n");
    printf("\t%s [-m] [-p] dir\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("ARGUMENTS:\n\n");
    printf("\tdir: path to directory\n\n");

    printf("OPTIONS:\n\n");
    printf("\t-m: use FTW_DEPTH\n");
    printf("\t-p: use FTW_PHYS\n");
}
