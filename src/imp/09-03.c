/*
SYNOPSIS

    Solution for exercise 09-03.

REQUIREMENT

    Implement initgroups() using setgroups() and library functions for
    retrieving information from the password and group files (section 8.4).
    Remember that a process must be privileged in order to be able to call
    setgroups().

SOLUTION

    Implement initgroups()

        - call getgrpent(), engrpent() and loop to  retrieving group
        - with each group loop though member to find member match with user
        - add group from function params to access list
        - call setgroup() to set access list

    Implement main()

    - verify arguments
    - try call initgroups()

USAGE

    $ ./dest/bin/09-03 kevin kevin

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#define _BSD_SOURCE

#include <sys/types.h>
#include <unistd.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <tlpiexer/error.h>

// see "man 3 initgroups"
int initgroups(const char *user, gid_t group);

void showhelp();

int main(int argc, char **argv)
{
    // variables
    int i;
    int gsize;
    gid_t gr[NGROUPS_MAX + 1];
    int group;

    // show help
    if (argc == 2 && strcmp(argv[1], "-h") == 0) {
        showhelp();
        return EXIT_SUCCESS;
    }

    // verify arguments
    if (argc != 3) {
        showhelp();
        return EXIT_FAILURE;
    }

    group = atoi(argv[2]);
    if (initgroups(argv[1], group) == -1)
        exit_err("initgroups");

    gsize = getgroups(NGROUPS_MAX + 1, gr);
    if (gsize == -1)
        exit_err("getgroups");

    printf("%s belong groups: ", argv[1]);
    for (i = 0; i < gsize; i++)
        printf("%ld ", (long)gr[i]);
    printf("\n");

    return EXIT_SUCCESS;
}

int initgroups(const char *user, gid_t grp)
{
    // variables
    struct group *gr;
    char **mem;
    gid_t grb[NGROUPS_MAX + 1];
    int gsize;

    // init
    gsize = 0;

    // walk though all of group and verify user belong
    for (gr = getgrent(); gr != NULL; gr = getgrent()) {
        for (mem = gr->gr_mem; *mem != NULL; mem++) {
            if (strcmp(*mem, user) == 0) {
                grb[gsize] = gr->gr_gid;
                gsize += 1;
            }
        }
    }
    endgrent();
    grb[gsize++] = grp;

    // set group
    if (setgroups(gsize, grb) == -1)
        return -1;

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "9-3";

    printf("\nUSAGE:\n\n");
    printf("\t%s user group\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("ARGUMENTS:\n\n");
    printf("\tuser: user to call initgroups()\n"); 
    printf("\tgroup: group to call initgroups()\n\n"); 
}
