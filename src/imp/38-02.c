/*
SYNOPSIS

    Solution for exercise 38-02.

REQUIREMENT

    Write a set-user-id-root program similar to the sudo(8) program. 
    This program should take command-line options and arguments as follow:

        $ ./douser [-u user] program-file arg1 arg2 ...

    The douser program executes program-file, whith the given arguments, as 
    though it was run by user (If the -u option is omitted, then user should
    default to root). Before executing program-file, douser should request
    the password for user, authenticate tit against the standard password
    file (see example 8-2, in summary), and then set all of the process user
    and group ids to the correct values for that user.

SOLUTION

    - parse options
    - verify arguments, options
    - request for password
    - verify password. if not match, exit with error. if match, continue
    - make current process belong of request user by setresuid()
    - perform execvp() to execute ivoked program

USAGE

    $ ./dest/bin/38-02 ls /root
    password: 
    data.txt

    $ ./dest/bin/38-02 -u kevin ls /home/kevin
    password: 
    desktop    downloads  music public     videos

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#define _BSD_SOURCE
#define _XOPEN_SOURCE
#define _GNU_SOURCE

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <shadow.h>

#include <tlpiexer/error.h>

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

int main(int argc, char *argv[])
{
    // variables
    char *username, *password, *encryped;
    struct passwd *pwd;
    struct spwd *spwd;
    int i;
    char *cmd, **cmd_args;

    // init
    username = NULL;

    // no match with any execute case
    if (argc < 2) {
        showhelp();
        return EXIT_FAILURE;
    }

    // show help
    if (argc == 2 && strcmp(argv[1], "-h") == 0) {
        showhelp();
        return EXIT_SUCCESS;
    }

    // execute with specific user
    // format <cmd> -u user command
    if (argc >= 4 && strcmp(argv[1], "-u") == 0) {
        username = argv[2];
        cmd = argv[3];
        cmd_args = argv + 3;
    } 
    
    // execute with root user
    else if (argc >= 2){
        cmd = argv[1];
        cmd_args = argv + 1;
    }
 
    // execute with default user, root
    // format <cmd> command
    if (argc >= 2 && strcmp(argv[1], "-u") != 0)
        username = strdup("root");

    // retrieve password entry for username
    // if shadow password is existed, use it instead of password
    if ((pwd = getpwnam(username)) == NULL) {
        fprintf(stderr, "username is not exist\n");
        return EXIT_FAILURE;
    }
    if ((spwd = getspnam(username)) == NULL && errno == EACCES)
        exit_err("getspnam");
    if (spwd != NULL)
        pwd->pw_passwd = spwd->sp_pwdp;

    // request for password and verify
    if ((password = getpass("password: ")) == NULL)
        exit_err("getpass");
    if ((encryped = crypt(password, pwd->pw_passwd)) == NULL)
        exit_err("crypt");
    for (i = 0; password[i] != '\0'; i++)
        password[i] = 0;
    if (strcmp(encryped, pwd->pw_passwd) != 0) {
        fprintf(stderr, "incorrect password\n");
        return EXIT_FAILURE;
    }

    // make process to correct user id
    if (setresuid(pwd->pw_uid, pwd->pw_uid, pwd->pw_uid) == -1)
        exit_err("setresuid");

    // execute invoked program
    execvp(cmd, cmd_args);

    // if process continue here, error occur
    exit_err("execvp");
}

void showhelp()
{
    // variables
    char exename[] = "38-2";

    printf("\nUSAGE:\n\n");
    printf("\t%s command\n", exename);
    printf("\t%s -u user command\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("ARGUMENTS:\n\n");
    printf("\tcommand: command to execute as user alias\n\n");

    printf("OPTIONS:\n\n");
    printf("\t-u: username to execute command on\n");
    printf("\t    if it is not set, default is root user\n\n");
}
