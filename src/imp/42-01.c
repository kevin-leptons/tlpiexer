/*
SYNOPSIS

    Solution for exercise 42-01.

REQUIREMENT

    Write a program to verity that if a library closed with dlclose(), it is
    not unloaded if any of its symbols are ysed by another library.

SOLUTION

    - create an library called libtlpi421a
    - create an library called libtlpi421b, it use symbols of libtlpi421a
    - load libtlpi421a, libtlpi421b by dlopen(), find symbol in libtlpi421a
      by dlsym() expect that found, libtlpi421a contain thats symbol
    - close libtlpi421a by dlclose(), find symbol in libtlpi421a by dlsym()
      expect that found, libtlpi421a is not unload because libtlpi421b use it
    - close libtlpi421b by dlclose(), find symbol in libtlpi421a by dlsym()
      expect that not found, libtlpi421a is unload because libtlpi421b is 
      unloaded 

USAGE

    $ ./dest/bin/42-01
    exec: load libtlpi421a, libtlpi421b
    exec: found tlpi421a_fn1 in libtlpi421a
    exec: call libtlpi421a:tlpi421a_fn1
    lib: tlpi421a_fn1
    exec: close libltpi421a
    exec: still found tlpi421a_fn1 in libtlpi421a
    exec: call libtlpi421a:tlpi421a_fn1
    lib: tlpi421a_fn1
    exec: close libltpi421b
    exec: not found tlpi421a_fn1 in libtlpi421a

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <string.h>
#include <limits.h>
#include <libgen.h>

#include <tlpiexer/error.h>

#define LIB421A_NAME "libtlpi421a.so"
#define LIB421B_NAME "libtlpi421b.so"

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
    void *lib421a, *lib421b;
    void (*tlpi421a_fn1)(void);
    char lib_path[PATH_MAX];
    char liba_path[PATH_MAX];
    char libb_path[PATH_MAX];

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

    // prepare path to lib
    if (readlink("/proc/self/exe", lib_path, sizeof(liba_path)) <= 0) 
        exit_err("readlink:lib_path");
    if (dirname(lib_path) == NULL)
        exit_err("dirname:lib_path");
    strcat(lib_path, "/../lib/");
    strcpy(liba_path, lib_path); 
    strcat(liba_path, LIB421A_NAME);
    strcpy(libb_path, lib_path);
    strcat(libb_path, LIB421B_NAME);

    // dynamic load library
    printf("exec: load libtlpi421a, libtlpi421b\n");
    if ((lib421a = dlopen(liba_path, RTLD_NOW | RTLD_GLOBAL)) == NULL)
        exit_dlerr("dlopen");
    if ((lib421b = dlopen(libb_path, RTLD_NOW | RTLD_GLOBAL)) == NULL)
        exit_dlerr("dlopen");

    // obitan symbol of libtlpi421a
    if ((tlpi421a_fn1 = dlsym(lib421a, "tlpi421a_fn1")) == NULL)
        exit_dlerr("dlsym");
    printf("exec: found tlpi421a_fn1 in libtlpi421a\n");
    printf("exec: call libtlpi421a:tlpi421a_fn1\n");
    (*tlpi421a_fn1)();

    // close libtlpi421a
    // show libtlpi421a, libtlpi421b is unload from memory or not
    printf("exec: close libltpi421a\n");
    if (dlclose(lib421a) == -1)
        exit_dlerr("dlclose");
    if ((tlpi421a_fn1 = dlsym(lib421a, "tlpi421a_fn1")) == NULL)
        exit_dlerr("dlsym");
    printf("exec: still found tlpi421a_fn1 in libtlpi421a\n");
    printf("exec: call libtlpi421a:tlpi421a_fn1\n");
    (*tlpi421a_fn1)();

    // close libtlpi421b
    // show libtlpi421a, libtlpi421b is unload from memory or not
    printf("exec: close libltpi421b\n");
    if (dlclose(lib421b) == -1)
        exit_dlerr("dlclose");
    if ((tlpi421a_fn1 = dlsym(lib421a, "tlpi421a_fn1")) == NULL)
        printf("exec: not found tlpi421a_fn1 in libtlpi421a\n");
    else 
        printf("exec: found tlpi421a_fn1 in libtlpi421a\n");

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "42-1";

    printf("\nUSAGE:\n\n");
    printf("%s\n", exename);
    printf("%s -h\n\n", exename);
}

void exit_dlerr(const char *msg)
{
    printf("%s: %s\n", msg, dlerror());
    exit(EXIT_FAILURE);
}
