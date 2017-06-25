/*
SYNOPSIS

    Solution for exercise 42-02.

REQUIREMENT

    Add a dladdr() call to the program in listing 42-1 (dynload.c) in order to
    retrieve information about the address retrurned by dlsym(). Print out the
    values of the fields of the returned Dl_info structure, and verify that
    they are as expected.

SOLUTION

    - copy source code in tlpi/shlibs/dynload.c
    - use dladdr() to retrieve information of symbol
    - show information which retrieved

USAGE

    $ ./dest/bin/42-02 dest/lib/libtlpi421a.so tlpi421a_fn1
    exec: call tlpi421a_fn1() in dest/lib/libtlpi421a.so
    lib: tlpi421a_fn1
    exec: retrieve information of 0xe5894855
    info.fname = dest/lib/libtlpi421a.so
    info.fbase = 0x464c457f
    info.sname = tlpi421a_fn1
    info.saddr = 0x69706c74

AUTHORS

    Michael Kerrish
    Kevin Leptons <kevin.leptons@gmail.com>
*/

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : show error message by dynamic loader then exit process
//            with EXIT_FAILURE
//            message by format <msg>: error message by dlerror()
// arg      :
//  - msg: error message
// ret      : none
void exit_dlerr(const char *msg);

int main(int argc, char *argv[])
{
    // variables
    int opt;
    void *libhand;
    void (*funcp)(void);
    const char *dl_err;
    Dl_info info;

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

    // verify arguments
    if (argc != 3) {
        showhelp();
        return EXIT_FAILURE;
    }

    // load the shared library and get a handle for later use
    if ((libhand = dlopen(argv[1], RTLD_LAZY)) == NULL)
        exit_dlerr("dlopen");

    // search library for symbol named in argv[2]
    (void)dlerror();
    funcp = dlsym(libhand, argv[2]);
    dl_err = dlerror();
    if (dl_err != NULL) {
        fprintf(stderr, "dlsym: %s\n", dl_err);
        return EXIT_FAILURE;
    }

    // if the address returned by dlsym() is non-null try calling it
    // as a function that takes no arguments
    if (funcp == NULL) {
        printf("%s is NULL", argv[2]);
    } else  {
        printf("exec: call %s() in %s\n", argv[2], argv[1]);
        (*funcp)();
    }

    // retrieve and show information about symbol
    printf("exec: retrieve information of 0x%x\n", *(int*)funcp);
    if (dladdr((void*)funcp, &info) == 0)
        exit_dlerr("dladdr");
    printf("info.fname = %s\n", info.dli_fname);
    printf("info.fbase = 0x%x\n", *(int*)info.dli_fbase);
    printf("info.sname = %s\n", info.dli_sname);
    printf("info.saddr = 0x%x\n", *(int*)info.dli_sname);

    // close the library
    dlclose(libhand);

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "42-2";

    printf("\nUSAGE:\n\n");
    printf("%s lib function\n", exename);
    printf("%s -h\n\n", exename);

    printf("ARGUMENTS:\n\n");
    printf("lib: name or path to library\n");
    printf("function: function in library\n\n");
}

void exit_dlerr(const char *msg)
{
    printf("%s: %s\n", msg, dlerror());
    exit(EXIT_FAILURE);
}
