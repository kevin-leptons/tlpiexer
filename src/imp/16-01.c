/*
SYNOPSIS

    Solution for exercise 16-01.

REQUIREMENT

    Write a program that can be used to create or modify a user EA for a file
    (i.e., a simple version of setfattr(1)). The filename and the EA name and
    value should be supplied as command-line arguments to the program.

SOLUTION

USAGE

    # ensure that attr package was installed for testing
    $ apt-get install attr

    $ touch tmp/f.txt
    $ ./dest/bin/16-01 -n name tmp/f.txt
    $ getfattr -d tmp/f.txt
    # file: tmp/f.txt
    user.name=""

    $ ./dest/bin/16-01 -n email -v kevin.leptons@gmail.com tmp/f.txt
    $ getfattr tmp/f.txt
    # file: tmp/f.txt
    user.name=""
    user.email="kevin.leptons@gmail.com"

    $ ./dest/bin/16-01 -x email tmp/f.txt
    $ getfattr tmp/f.txt
    user.name=""

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/xattr.h>

#include <tlpiexer/error.h>

#define RET_FAILURE -1
#define RET_SUCCESS 0
#define EA_NAME_MAX 256

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : set file attribute belong user namespace
// arg      :
//  - path: path to file
//  - name: name of attribute
//  - value: value of attribute
// ret      :
//  - 0 on success
//  - -1 on error
int setattru(char *path, char *name, char *value);

// spec     : delete file attribute belong user namespace
// arg      :
//  - path: path to file
//  - name: name of attribute
// ret      :
//  - 0 on success
//  - -1 on error
int delattru(char *path, char *name);

int main(int argc, char **argv)
{
    // variables
    char opt;
    char op;
    char *name;
    char *value;

    // init
    op = 1;
    name = NULL;
    value = NULL;

    // parse arguments
    while ((opt = getopt(argc, argv, "hn:v:x:")) != -1) {
        switch (opt) {
            case 'h':
                showhelp();
                return EXIT_SUCCESS;
            case 'n':
                name = optarg;
                break;
            case 'v':
                value = optarg;
                break;
            case 'x':
                op = 2;
                name = optarg;
                break;
        }
    }

    // verify arguments
    if (op == 0 || (op == 2 && value != NULL)) {
        showhelp();
        return EXIT_FAILURE; 
    }

    // verify file
    if (optind >= argc) {
        showhelp();
        return EXIT_FAILURE;
    }

    // create/modify/delete attributes depend on operation
    switch (op) {
        case 1:
            if (setattru(argv[optind], name, value) == -1)
                exit_err("setattru");
            break;
        case 2:
            if (delattru(argv[optind], name) == -1)
                exit_err("delattru");
    }

    // success
    return EXIT_SUCCESS;
}

int setattru(char *path, char *name, char *value)
{
    // variables
    char rname[EA_NAME_MAX];
    char *rvalue;

    // init
    snprintf(rname, EA_NAME_MAX, "user.%s%c", name, 0);
    rvalue = value;

    // verify value
    if (rvalue == NULL)
        rvalue = "";

    // set
    if (setxattr(path, rname, rvalue, strlen(rvalue), 0) == -1)
        return RET_FAILURE;

    // success
    return RET_SUCCESS;
}

int delattru(char *path, char *name)
{
    printf("%s %s\n", path, name);
    // variables
    char rname[EA_NAME_MAX];

    // init
    snprintf(rname, EA_NAME_MAX, "user.%s%c", name, 0);

    // remove
    if (removexattr(path, rname) == -1)
        return RET_FAILURE;

    // success
    return RET_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "16-1";

    printf("\nUSAGE:\n\n");
    printf("\t%s -n name file\n", exename);
    printf("\t%s -n name -v value file\n", exename);
    printf("\t%s -x name file\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("ARGUMENTS:\n\n");
    printf("file: path to file\n\n");

    printf("OPTIONS:\n\n");
    printf("\t-n: name of attribute to set\n");
    printf("\t    this value is convert to format user.name\n");
    printf("\t-v: value of attribute to set\n");
    printf("\t-x: delete attribute\n\n");
}
