/*
SYNOPSIS

    Solution for exercise 15-04.

REQUIREMENT

    The access() system call checks permissions using the process's real user
    and group ids. Write a corresponding function that performs the checks
    according to the process's effective user and group ids.

SOLUTION

    Implement eaccess_x()

        - set real-user-id to effective-user-id by setresuid()
        - set real-group-id to effective-group-id by setresgid()
        - verify acess by access() and store result to ok
        - restore real-user-id by setresuid()
        - restore real-group-id by setresgid()
        - return ok value

    Implement main()

        - parse options
        - verify arguments, options
        - call eacces()
        - show result

USAGE

    # create file to test
    $ touch tmp/data.txt
    $ ./dest/bin/15-04 -rw tmp/data.txt
    $ ./dest/bin/15-04 -x tmp/data.txt
    Error: eaccess_x:Permission denied

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#define _GNU_SOURCE

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <tlpiexer/error.h>

#define RET_SUCCESS 0
#define RET_FAILURE -1

// spec     : verify access of file by effective user, group id
// arg      :
//  - fpath: path to file
//  - mod: F_OK, R_OK, W_OK, X_OK
// ret      :
//  - 0 on access granted
//  - -1 on access denined
int eaccess_x(char *fpath, int mode);

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

int main(int argc, char **argv)
{
    // variables
    char opt;
    int fmode;

    // init
    fmode = 0;
    
    // parse arguments
    while ((opt = getopt(argc, argv, "hrwx")) != -1) {
        switch (opt) {
            case 'h':
                showhelp();
                return EXIT_SUCCESS;
            case 'r':
                fmode |= R_OK;
                break;
            case 'w':
                fmode |= W_OK;
                break;
            case 'x':
                fmode |= X_OK;
                break;
            default:
                showhelp();
                return EXIT_FAILURE;
        }
    }

    // verify mode
    if (fmode == 0) {
        showhelp();
        return EXIT_FAILURE;
    }

    // verify file
    if (optind >= argc) {
        showhelp();
        return EXIT_FAILURE;
    }

    // verify
    if (eaccess_x(argv[optind], fmode) == -1)
        exit_err("eaccess_x");
    printf("File's mode is OK\n");
     
    // success
    return EXIT_SUCCESS;
}

int eaccess_x(char *fpath, int mode)
{
    // variables
    int ok;
    uid_t uid, euid;
    gid_t gid, egid;

    // init
    uid = getuid();
    euid = geteuid();
    gid = getgid();
    egid = getegid();

    // set uid by euid
    if (setresuid(euid, -1, -1) == -1)
        return RET_FAILURE;

    // set gid by egid
    if (setresgid(egid, -1, -1) == -1)
        return RET_FAILURE;

    // verify access
    ok = access(fpath, mode);

    // set back uid
    if (setresuid(uid, -1, -1) == -1)
        return RET_FAILURE;

    // set back gid
    if (setresgid(gid, -1, -1) == -1)
        return RET_FAILURE;

    // result
    return ok;
}

void showhelp()
{
    // variables
    char exename[] = "15-4";

    printf("\nUSAGE:\n\n");
    printf("\t%s [-r] [-w] [-x] file\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("OPTIONS:\n\n");
    printf("\t-r: verify effective user have read permision\n"); 
    printf("\t-w: verify effective user have write permision\n"); 
    printf("\t-x: verify effective user have execute permision\n"); 
}
