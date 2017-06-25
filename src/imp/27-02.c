/*
SYNOPSIS

    Solution for exercise 27-02.

REQUIREMENT

    Use execve() to implement execlp(). You will need to use the stdarg(3) api
    to handle the variable-length argument list supplied to execlp(). You will
    also need to use function in the malloc package to allocate space for
    argument and environemnt vectors. Finally, note that an easy way of 
    checking whether a file exists in a particular directory and is executable
    is simply to try execing the file.

SOLUTION

    Implement execlp()

        - resolve real path to executable file. if exepath is path, verify
          file is exist and executable. if exepath is file, verify file with
          environment variable PATH
        - prepare arguments, environment variables
        - call execve()
        - if process continue, verfiy error and free memory

    Impelement main()

        - verify arguments
        - call execlp()

USAGE

    $ ./dest/bin/27-02 /bin/ls .
    asset   box.entry.sh       _config.yml  dest     install     src
    box cmake-build-debug  core     doc  license.md  tlpi
    boxctl  CMakeLists.txt     ctl      include  readme.md   tmp

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <tlpiexer/error.h>

#define RET_SUCCESS 0
#define RET_FAILURE -1

// spec     : contains environemtn variables 
extern char **environ;

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : execute file specify by filename and give it arguments from
//            arg
// arg      :
//  - filename: file name or path to file
//  - arg: list argument, end by null
//            if success, it never return
int execlp(const char *filename, const char *arg, ...);

int main(int argc, char **argv)
{
    // variables
    int opt;

    // verify arguments
    if (argc != 3) {
        showhelp();
        return EXIT_FAILURE;
    }

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

    // use execlp()
    execlp(argv[1], argv[1], argv[2], (char*)NULL);

    // if process continue here, call execlp() has error 
    exit_err("execlp");
    perror("execlp");
}

int execlp(const char *filename, const char *arg, ...)
{
    // variables
    va_list argp;
    char path[PATH_MAX];
    int argc;
    char *argv[127];
    char **envp;
    int i;
    char *pstart;
    char *epath;
    size_t cspath_size;
    struct stat fstat;
    int exe;

    // init
    argc = 0;
    exe = 0;
    memset(argv, 0, sizeof(argv));

    // switch use absolute path or relative path with PATH
    if (strchr(filename, '/') != NULL) {
        // exist '/' in filename
        // use absolute path
        strcpy(path, filename);
    } else {
        // not exist '/' in filename
        // search file from PATH
        
        // get PATH form environment variable
        epath = getenv("PATH");

        // if PATH is not exist
        // use default value from confgstr(_CS_PATH)
        if (epath == NULL) {
            cspath_size = confstr(_CS_PATH, NULL, (size_t)0);
            epath = malloc(cspath_size);
            if (epath == NULL)
                exit_err("malloc");
            confstr(_CS_PATH, epath, cspath_size);
        }
   
        // find executable file from PATH
        // use firt match
        pstart = epath;
        for (i = 0; i < strlen(epath); i++) {

            // skip until match split  character
            if (epath[i] != ':')
                continue;

            // create absolute path from PATH and filename
            memcpy(path, pstart, epath + i - pstart);
            path[epath + i - pstart] = '\0';
            strcat(path, "/");
            strcat(path, filename);
            pstart = epath + i + 1;

            // try get status of file
            // skip for error and continue find other path
            // if error, errono will be set
            // in last statement, exe = 0 mean that no executable file has found
            if (stat(path, &fstat) == -1)
                continue;

            // found file, but it is not executable
            // skip to find other path
            // in last statement, exe = 0 mean that no executable file has found
            if (!(fstat.st_mode & S_IXUSR || fstat.st_mode & S_IXGRP ||
                    fstat.st_mode & S_IXOTH))
                continue;

            // mark file is executable
            // and exit
            exe = 1;
            break;
        }

        // verify found executable file
        if (!exe)
            return RET_FAILURE;
    }
    
    // prepare argument
    if (arg != NULL) {
        argv[argc] = (char*)arg;
        argc++;

        va_start(argp, arg); 
        for (;;) {
            argv[argc] = va_arg(argp, char*);
            if (argv[argc] == NULL)
                break;
            argc++;
        }
        va_end(argp);
    }

    // prepare environment variables
    for (i = 0; environ[i] != NULL; i++) {
    }
    envp = malloc(sizeof(char*)*(i + 1));
    envp[i] = NULL;
    for (i--; i >= 0; i--) {
        envp[i] = strdup(environ[i]);
    }

    // use execve()
    execve(path, argv, envp);

    // if process continue here, call execve() has error
    // release memory and report error
    for (i = 0; argv[i] != NULL; i++) 
        free(argv[i]);
    for (i = 0; envp[i] != NULL; i++)
        free(envp[i]);
    free(envp);

    return RET_FAILURE;
}

void showhelp()
{
    // variables
    char exename[] = "27-2";

    printf("\nUSAGE:\n\n");
    printf("\t%s exepath argument\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("ARGUMENTS:\n\n");
    printf("\texepath: path to program to execute\n");
    printf("\targument: argument tranfer to program specify in exepath\n\n");
}
