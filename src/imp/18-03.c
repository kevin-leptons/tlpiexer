/*
SYNOPSIS

    Solution for exercise 18-03.

REQUIREMENT

    Implement realpath().

SOLUTION

    - parse options
    - verify arguments, options
    - loop to get real path from original path

USAGE

    # work in temporary directory because link is relative
    $ cd tmp
    $ pwd
    /mnt/workspace/package/tlpiexer/tmp

    $ touch f.txt
    $ ln -s f.txt l1.txt
    $ ln -s l1.txt l2.txt
    $ ln -s l2.txt l3.txt
    $ ln -s /mnt/workspace/package/tlpiexer/tmp/l3.txt l4.txt

    $ ./dest/bin/18-03 l4.txt
    /mnt/workspace/package/tlpiexer/tmp/f.txt

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <tlpiexer/error.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

void showhelp();
char *realpath_x(const char *path, char *resolved_path);

// accept absolute path, start with '/', remove all dot '..', '.'
char *solve_dot(char *path);

int main(int argc, char **argv)
{
    // variables
    char opt;
    char rpath[PATH_MAX];

    // verify path
    if (argc != 2) {
        showhelp();
        return EXIT_FAILURE;
    }

    // find options
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

    if (realpath_x(argv[1], rpath) == NULL)
        exit_err("realpath_x");
    printf("%s\n", rpath);

    return EXIT_SUCCESS;
}

char *realpath_x(const char *path, char *resolved_path)
{
    char fpath[PATH_MAX];
    char dpath[PATH_MAX];
    char lpath[PATH_MAX];
    struct stat f_stat; 
    ssize_t psize;

    // detect absolute path 
    // it's required because symbol can be relative
    if (path[0] == '/') {
        strcpy(fpath, path);
    } else {
        if (getcwd(fpath, sizeof(dpath)) == NULL)
            return NULL;
        strcat(fpath, "/");
        strcat(fpath, path);
    }

    // solve symbol link until match regular file
    for (;;) {
        strcpy(dpath, fpath);
        if (dirname(dpath) == NULL)
            return NULL;

        if (lstat(fpath, &f_stat) != 0)
            return NULL;
        if (!S_ISLNK(f_stat.st_mode))
            break;

        psize = readlink(fpath, lpath, sizeof(lpath));
        if (psize < 0)
            return NULL;
        lpath[psize] = 0;

        if (lpath[0] == '/') {
            strcpy(fpath, lpath);
        } else {
            strcpy(fpath, dpath);
            strcat(fpath, "/");
            strcat(fpath, lpath);
        }
    }

    // solve dot in path
    if (solve_dot(fpath) == NULL)
        return NULL;

    // copy result to target
    if (resolved_path == NULL) {
        resolved_path = malloc(strlen(fpath) + 1);
        if (resolved_path == NULL)
            return NULL;
    }
    strcpy(resolved_path, fpath);

    return resolved_path;
}

char *solve_dot(char *path)
{
    int i;
    int j;
    int jx;
    int jy;
    int offset;
    int path_len;

    if (path[0] != '/')
        return NULL;

    path_len = strlen(path);

    for (i = 0, j = 0, jx = 0, jy = 0; i < path_len; i++, j++) {
        if (path[i] != '/') {
            path[j] = path[i];
            continue;
        }

        offset = j - jy;

        // '..' to up dir
        if (offset == 3) {
            if (path[jy + 1] == '.' && path[jy + 2] == '.') {
                j = jx;
                jy = jx;
                for (jx--; path[jx] != '/'; jx--);
            }
        } 
        
        // '.' to current dir
        else if (offset == 2) {
            if (path[jy + 1] == '.')
                j = jy;
        }

        // double slash, merge it to one slash
        else if (offset == 1) {
            j = jy;
        }
        
        // other case, it is file name
        else {
            jx = jy;
            jy = j;
            path[j] = '/';
        }
    }

    path[j] = 0;
    return path;
}

void showhelp()
{
    // variables
    char exename[] = "18-3";

    printf("\nUSAGE:\n\n");
    printf("\t%s path\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("ARGUMENTS:\n\n");
    printf("\tpath: path to file\n");
}
