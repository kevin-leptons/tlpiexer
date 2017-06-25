#ifndef __TLPIEXER_59_1_H__
#define __TLPIEXER_59_1_H__

#include <stdbool.h>
#include <stdlib.h>

#define BUF_SIZE 2048
#define PORT_NUM "5000"
#define INT_LEN 30

struct lbuf
{
    int fd;
    bool efile;
    char buf[BUF_SIZE];
    char *cur;

    char *ebuf;
};

struct request
{
    size_t seq_num;
};

struct response
{
    size_t seq_num;
};

int init_lbuf(int fd, struct lbuf *lbuf);
int read_lbuf(struct lbuf *lbuf, char *line, size_t size);

#endif
