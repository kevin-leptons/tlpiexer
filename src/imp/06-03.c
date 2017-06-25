/*
SYNOPSIS

    Solution for exercise 06-03.

REQURIEMENT

    Implement setenv() and unsetenv() using getenv(), putenv(), and, where
    necessary, code that directly modifies environ. Your version of unsetenv()
    should check to see whether there are multiple definitions of an 
    environment variable, and remove them all (Which is what glibc version 
    of unsetenv() does).

SOLUTION

    Implement setenv()

        - verify variable name has early exists
        - call malloc() for environemnt string
        - copy string format name=value to above allocatd memory
        - call putenv() to append new string to environment string

    implement unsetenv()

        - verify variable name has early exists
        - loop to find and remove environment string

    Implement main()

        - verify arguments, options
        - switch to call setenv() or unsetenv()

USAGE

    $ ./dest/bin/06-03 set MY_CAR "Bad car"
    ENGINE_NAME=Turbo v8
    MY_CAR=Bad car

    $ ./dest/bin/06-03 uset ENGINE_NAME
    env ENGINE_NAME: cleared

AUTHOR   : kevin.leptons@gmail.com
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <tlpiexer/error.h>

#define DEF_ENV_NAME "ENGINE_NAME"
#define DEF_ENV_VALUE "Turbo v8"

// see "man 7 environ"
extern char **environ;

// see "man 3 setenv"
int setenv(const char *name, const char *value, int overwrite);
int unsetenv(const char *name);

void showhelp();

int main(int argc, char **argv)
{
    // variables
    char *env_val;

    // show help
    if (argc < 2 || strcmp(argv[1], "-h") == 0) {
        showhelp();
        return EXIT_SUCCESS;
    }

    // verify argument
    if (argc < 3) {
        showhelp();
        return EXIT_FAILURE;
    }

    // set default env
    if (setenv(DEF_ENV_NAME, DEF_ENV_VALUE, 0) == -1)
        exit_err("setenv:DEF_ENV_NAME");
    
    // set env
    if (strcmp(argv[1], "set") == 0) {
        // verify argument
        if (argc != 4) {
            showhelp();
            return EXIT_FAILURE;
        }

        if (setenv(argv[2], argv[3], 0) == -1)
            exit_err("setenv");

        // show environment variables
        env_val = getenv(DEF_ENV_NAME);
        if (env_val == NULL)
            fprintf(stderr, "not found environ by name: %s\n", DEF_ENV_NAME);
        else
            printf("%s=%s\n", DEF_ENV_NAME, env_val);

        env_val = getenv(argv[2]);
        if (env_val == NULL)
            fprintf(stderr, "not found environ by name: %s\n", argv[2]);
        else
            printf("%s=%s\n", argv[2], env_val);

    }

    // unset env
    else if (strcmp(argv[1], "uset") == 0) {
        if (unsetenv(argv[2]) == -1)
            exit_err("unsetenv");

        // show environment variables
        env_val = getenv(DEF_ENV_NAME);
        if (env_val == NULL)
            printf("env %s: cleared\n", DEF_ENV_NAME);
        else
            fprintf(stderr, "env %s still exist\n", DEF_ENV_NAME);
    }

    // unknow command
    else {
        fprintf(stderr, "unknow command: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    // success
    return EXIT_SUCCESS;
}

int setenv(const char *name, const char *value, int overwrite)
{
    // variables
    char *senv;

    // verify exist variable name
    if (getenv(name) != NULL && overwrite == 0) {
        return 0;
    } 

    // create string environment
    senv = malloc(strlen(name) + strlen(value) + 1);
    if (senv == NULL) {
        return -1;
    }
    strcpy(senv, name);
    strcat(senv, "=");
    strcat(senv, value);

    // put string to environemnt
    if (putenv(senv) != 0) {
        return -1;
    }

    // success
    return 0;
}

int unsetenv(const char *name)
{
    // variables
    int i = 0;

    // verify variable
    if (getenv(name) == NULL)
        return 0;

    // remove all variable have name
    while(environ[i] != NULL) {
        if (memcmp(environ[i], name, strlen(name)) == 0 &&
                environ[i][strlen(name)] == '=') {
            environ[i] = NULL;
        } 
        i++;
    }

    // success
    return 0;
}

void showhelp()
{
    // variables
    char exename[] = "6-3";

    printf("\nUSAGE:\n\n");
    printf("\t%s set name value\n", exename);
    printf("\t%s uset name\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("COMMANDS:\n\n");
    printf("\tset: set environment variable\n"); 
    printf("\tuset: unset environment variable\n\n"); 

    printf("ARGUMENTS:\n\n");
    printf("\tname: name of environment variable to set or unset\n"); 
    printf("\tvalue: value of environment variable to set\n\n"); 
}
