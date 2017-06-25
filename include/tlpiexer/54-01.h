#ifndef __TLPIEXER_54_1_H__
#define __TLPIEXER_54_1_H__

#include <sys/types.h>

#define SEM_READ_NAME "/TLPIEXER-54-1-sem-read"
#define SEM_WRITE_NAME "/TLPIEXER-54-1-sem-write"
#define SHARED_MEM_NAME "/TLPIEXER-54-1-shared-mem"
#define BUF_SIZE 1024

struct shared_mem
{
    ssize_t size;
    char data[BUF_SIZE];
};

#endif
