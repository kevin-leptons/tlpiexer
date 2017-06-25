/*
SYNOPSIS

    Solution for excercise 09-04.

REQUIREMENT

    If a process whose user ids all have the value X executes a set-user-id
    program whose user id, Y, is nonzero, then the process ccredential are set
    as follows

        real=X effective=Y saved=Y

    (We ignore the file-system user id, since it tracks the effective user id)
    show the setuid(), setreuid(), and setresuid() calls, respectively, that
    would be used to perfrom the following operations

        a) suppend and resume the set-user-id identity (i.e., switch the
        effective user id to the value of the real user id and then back to
        the saved set-user-id

        b) permanently drop the set-user-id identity (i.e.m ensure that the
        effective user id and the saved set-user-id are set to the value of
        the real user id)

    (This exercise also requires the use of getuid() and geteuid() to retrieve
    the process's real and effective user ids) note that for certain of the
    system calls listed above, some of these operations can't be performed

SOLUTION

    Do as requirement.

USAGE

    # enable user-set-id
    $ chmod u+s,g+s dest/bin/09-04

    # logging as other user then execute this program
    $ ./dest/bin/09-04
    start ids:
    r=1000 e=1000 s=1000
    suspend: seteuid(r)
    r=1000 e=1000 s=1000
    resume: seteuid(s)
    r=1000 e=1000 s=1000
    drop permanetly: setresuid(-1, r, r)
    r=1000 e=1000 s=1000
    resume: setresuid(-1, e, s)
    r=1000 e=1000 s=1000

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#define _GNU_SOURCE

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

// spec     : show ids to stdout
// arg      : none
// ret      : none
void showids(void)
{
    printf("r=%i e=%i s=%i\n", getuid(), geteuid(), geteuid());
}

int main(int argc, char **argv)
{
    // variables
    uid_t r, e, s;

    // show start ids
    printf("start ids:\n");
    r = getuid();
    e = geteuid();
    s = e;
    showids();

    // suspend, show ids
    printf("suspend: seteuid(r)\n");
    if (seteuid(r) == -1) {
        perror("seteuid");
        return EXIT_FAILURE;
    }
    showids();

    // resume, show ids
    printf("resume: seteuid(s)\n");
    if (seteuid(s) == -1) {
        perror("seteuid");
        return EXIT_FAILURE;
    }
    showids();

    // permanently drop the set-user-id
    printf("drop permanetly: setresuid(-1, r, r)\n");
    if (setresuid(-1, r, r) == -1) {
        perror("setresuid");
        return EXIT_FAILURE;
    }
    showids();

    // resume, show ids
    printf("resume: setresuid(-1, e, s)\n");
    if (setresuid(-1, e, s) == -1) {
        perror("setresuid");
        return EXIT_FAILURE;
    }
    showids();

    // success
    return EXIT_SUCCESS;
}
