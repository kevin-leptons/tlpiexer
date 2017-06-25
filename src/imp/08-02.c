/*
SYNOPSIS

    Solution for exercise 08-02.

REQUIREMENT

    Implement getpwnam() using setpwent(), getpwent(), and endpwent().

SOLUTION

    Implement getpwnam()

        - use setpwent(), getpwent(), endpwent() and loop to find password 
          entry match with username
        - verify finding result
        - copy password entry has found to static memory
        - return address of static memory

    Implement main()

        - verify arguments
        - call getpwnam() to reteview password entry
        - show password entry information

USAGE

    $ ./dest/bin/08-02 kevin
    kevin     1000 /bin/bash

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <pwd.h>
#include <string.h>

// spec     : store password information returned by getpwnam()
static struct passwd getpwnam_passwd;

// spec     : get information about password
// arg      :
//  - name: username
// ret      :
//  - pointer to static data contains information on success
//  - null on error
struct passwd *getpwnam(const char *name);

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

int main(int argc, char **argv)
{
    // variables
    struct passwd *pwd;

    // show help
    if (argc == 2 && strcmp(argv[1], "-h") == 0) {
        showhelp();
        return EXIT_SUCCESS;
    }

    // verify arguments
    if (argc != 2) {
        showhelp();
        return EXIT_FAILURE;
    }

    // get information
    pwd = getpwnam(argv[1]);
    if (pwd == NULL) {
        fprintf(stderr, "not found user: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    // print
    printf("%-8s %5i %s\n", pwd->pw_name, pwd->pw_uid, pwd->pw_shell);

    // success
    return EXIT_SUCCESS;
}

struct passwd *getpwnam(const char *name)
{
    // variables
    struct passwd *pwd;

    // find password entry match with username
    while((pwd = getpwent()) != NULL) {
        if (strcmp(pwd->pw_name, name) == 0) {
            break;
        }
    }
    endpwent();

    // verify finding result
    if (pwd == NULL)
        return NULL;

    // copy value of password entry to static memory
    getpwnam_passwd = *pwd;

    // success
    return &getpwnam_passwd;
}

void showhelp()
{
    // variables
    char exename[] = "8-2";

    printf("\nUSAGE:\n\n");
    printf("\t%s username\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("ARGUMENTS:\n\n");
    printf("\tusername: user name to view information\n\n"); 
}
