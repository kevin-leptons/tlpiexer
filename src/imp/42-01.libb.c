// SPEC     :
//
// libtlpi421b. this library contains some symbols to demonstrate dynamic
// loader of exercise 42-1. it must depend on libtlpi421a
//
// AUTHOR   : kevin.leptons@gmail.com

#include <stdio.h>

#include <tlpiexer/42-01-liba.h>

void tlpi421b_fn1(void)
{
    tlpi421a_fn1();
    printf("lib: tlpi421b_fn1\n");
}
