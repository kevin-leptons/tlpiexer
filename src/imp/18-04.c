/*
SYNOPSIS

    Solution for exercise 18-04.

REQUIREMENT

    Modify the program in listing 18-2 (list_files.c) to use readdir_r()
    instead of readdir().

SOLUTION

    - copy source code in tlpi/dirs_links/list_files.c
    - reqplace readir() by readdir_r()

USAGE

    $ ./dest/bin/18-04 .
    include
    CMakeLists.txt
    license.md
    tmp
    ctl
    box.entry.sh
    doc
    .idea
    .ycm_extra_conf.py
    .gitignore
    .ycm_extra_conf.pyc
    cmake-build-debug
    .readme.md.swp
    src
    dest
    readme.md
    .git
    tlpi
    core
    install
    asset
    boxctl
    box

AUTHORS

    Michael Kerrisk
    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

// spec     : list content of directory
// stdout   : sub name of file or directory in each line
// ret      : none
static void listfiles(const char *dirpath);

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

int main(int argc, char **argv)
{
    // variables
    char opt;
    int i;

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

    // no argument, use current working directory
    if (optind >= argc)
        listfiles(".");
    else
        for (i = optind; i < argc; i++)
            listfiles(argv[i]);  

    // success
    return EXIT_SUCCESS; 
}

static void listfiles(const char *dirpath)
{
    // variables
    DIR *dir;
    struct dirent de, *result;

    // open dir
    dir = opendir(dirpath);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    // foreach entry in this directory, print directory + filename
    for (;;) {
        errno = 0;
        if (readdir_r(dir, &de, &result) != 0)
            break;

        if (result == NULL)
            break;

        // skip link to cwd and up dir
        if (strcmp(de.d_name, ".") == 0 || strcmp(de.d_name, "..") == 0)
            continue;

        // show file name
        printf("%s\n", de.d_name);
    }

    // verify error
    if (errno != 0)
        perror("readdir_r");

    // close dir
    if (closedir(dir) == -1)
        perror("closedir");
}

void showhelp()
{
    // variables
    char exename[] = "18-4";

    printf("\nUSAGE:\n\n");
    printf("\t%s dir\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("ARGUMENTS:\n\n");
    printf("\tdir: path to directory to see file\n\n");
}
