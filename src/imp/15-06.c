/*
SYNOPSIS

    Solution for exercise 15-06.

REQUIREMENT

    The chmod a+rX file command enables read permission for all categories
    of user, and likewise enables execute permission for all categories of user
    if file is an directory or execute permission is enabled for any of the
    user categories for file, as demonstrated in the follow example.

        $ ls -ld dir file prog
        dr-------- 2 mtk users      48 May  4 12:28 dir
        -r-------- 1 mtk users   19794 May  4 12:22 file
        -r-x------ 1 mtk users   19336 May  4 12:21 prog

        $ chmod a+rX dir file prog
        $ ls -ld dir file prog
        dr-xr-xr-x 2 mtk users   48    May  4 12:28 dir
        -r--r--r-- 1 mtk users   19794 May  4 12:22 file
        -r-xr-xr-x 1 mtk users   19336 May  4 12:21 prog

    write a program that use stat() and chmod() to perform the equivalent of
    chmod a+rX.

SOLUTION

    Implement chmodx()

        - reterview file status
        - modify mode: all execute if file is directory
        - modify mode: all execute if any execute was  enable
        - change file mode

    Implement main()

        - parse options
        - verify arguments, options
        - call chmodx()

USAGE

    # prepare files

    $ mkdir tmp/ax/dir
    $ chmod -rwx tmp/ax/dir

    $ touch tmp/ax/file
    $ chmod -rw,u+r tmp/ax/file

    $ touch tmp/ax/prog
    $ chmod -rw,u+rx tmp/ax/prog

    $ ls -l tmp/ax
    d--------- 2 kevin kevin 4096 Jun 12 21:32 dir
    -r-------- 1 kevin kevin    0 Jun 12 21:41 file
    -r-x------ 1 kevin kevin    0 Jun 12 21:42 prog

    # this command is same as "chmod a+rX"
    $ ./dest/bin/15-06 tmp/ax/dir tmp/ax/file tmp/ax/prog

    $ ls -l tmp/ax
    dr-xr-xr-x 2 kevin kevin 4096 Jun 12 21:32 dir
    -r--r--r-- 1 kevin kevin    0 Jun 12 21:41 file
    -r-xr-xr-x 1 kevin kevin    0 Jun 12 21:42 prog

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <tlpiexer/error.h>

#define RET_SUCCESS 0
#define RET_FAILURE -1

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : enable read permission for all of users. if file is directory or
//            any executable has enable, enable executable for all of users 
// arg      :
//  - fpath: path to file
// ret      :
//  - 0 on success
//  - -1 on error
int chmodx(char *fpath);

int main(int argc, char **argv)
{
    // variables
    char opt;
    int i;

    // parse arguments
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

    // verify file
    if (optind >= argc) {
        showhelp();
        return EXIT_FAILURE;
    }

    // chmodx for each file
    for (i = optind; i < argc; i++)
        if (chmodx(argv[i]) == -1)
            exit_err("chmodx");

    // success
    return EXIT_SUCCESS;
}

int chmodx(char *fpath)
{
    // variables
    int fmode;
    struct stat s;

    // init
    fmode = S_IRUSR | S_IRGRP | S_IROTH;

    // reterview file status
    if (stat(fpath, &s) == -1)
        return RET_FAILURE;

    // modify mode: all execute if file is directory
    if (S_ISDIR(s.st_mode))
        fmode |= S_IXUSR | S_IXGRP | S_IXOTH;

    // modify mode: all execute if any execute was enable
    if (s.st_mode & S_IXUSR || s.st_mode & S_IXGRP || s.st_mode & S_IXOTH)
        fmode |= S_IXUSR | S_IXGRP | S_IXOTH;

    // change file mode
    if (chmod(fpath, fmode) == -1)
        return RET_FAILURE;

    // success
    return RET_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "15-6";

    printf("\nUSAGE:\n\n");
    printf("%s\n", exename);
    printf("%s -h\n\n", exename);
}
