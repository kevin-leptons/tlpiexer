/*
SYNOPSIS

    Solution for exercise 40-01.

REQUIREMENT

    Implement getlogin(). As noted in section 40.5, getlogin() may not work
    correctly for processes running under some software terminal emulators;
    in this case, test from a virtual console instead.

SOLUTION

    Implement getlogin():

        - sure that template file use for utmp function is PATH_UTMP by call
          utmpname()(STDIN_FILENO, ut.
        - retrieve identity of terminal by ttyname()
        - use setutxend(), endutxent(), getutxline() to find user logged in
          current terminal
        - ouput username

    Implement main():

        - call getlogin() to get username which current logged in user
        - show username 

USAGE    :

    # use virtual console by Ctrl + Alt + F1 instead of terminal emulators
    # use Ctrl + Alt + F7 to back to GUI 
    $ ./dest/bin/40-01
    current logged user: kevin

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <paths.h>
#include <utmp.h>
#include <utmpx.h>
#include <libgen.h>

#include <tlpiexer/error.h>

// spec     : this is global static memory use for getlogin()
static char getlogin_username[UT_NAMESIZE];

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : retrieve username logged in current terminal
// arg      : none
// ret      :
//  - pointer to static string, contain username on success
//  - NULL on not found or error
char *getlogin(void);

int main(int argc, char *argv[])
{
    // variables
    int opt;
    char *username;

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

    // verify no arguments has provided
    if (argc > 1) {
        showhelp();
        return EXIT_FAILURE;
    }

    // retrive and show logged username
    if ((username = getlogin()) == NULL)
        exit_err("getlogin");
    printf("current logged user: %s\n", username);

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "40-1";

    printf("\nUSAGE:\n\n");
    printf("%s\n", exename);
    printf("%s -h\n\n", exename);
}

char *getlogin(void)
{
    // variables
    struct utmpx ut_find;
    struct utmpx *ut_match;
    char *ttypath, *ttyid;

    // init
    memset(&ut_find, 0, sizeof(ut_find));

    // sure that template file use for utmp function is PATH_UTMP
    if (utmpname(_PATH_UTMP) == -1)
        return NULL;

    // retrieve identity of current terminal
    // returned from ttyname() have format '/dev/../name'
    // we need put only name into ut_line
    if ((ttypath = ttyname(STDIN_FILENO)) == NULL)
        return NULL;
    ttyid = basename(ttypath);
    strcpy(ut_find.ut_line, ttyid);

    // fine record which match with identify of current terminal
    setutxent();
    ut_match = getutxline(&ut_find);
    endutxent();
    if (ut_match == NULL)
        return NULL;

    // save username to static memory
    strcpy(getlogin_username, ut_match->ut_user);

    // success
    return getlogin_username;
}
