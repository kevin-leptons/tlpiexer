#ifndef __TLPIEXER_61_6_H__
#define __TLPIEXER_61_6_H__

#include <poll.h>

#define MAX_OBS_SOCK_NUM 1024

struct oob_sock 
{
    // 0 - data_fd, 1 - oob_fd
    struct pollfd fds[2]; 
};

enum oob_cmd
{
    OOB_OK = 0,
    OOB_CLOSE = 1,
    OOB_DATA = 2
};

struct handshake_msg
{
    char key[16];
    int port;
};

struct oob_msg
{
    enum oob_cmd code;
    char data[256];
};

/*
Create out of band socket pair.
*/
void mk_oob_sock(int data_sock, int oob_sock, struct oob_sock *obs);

/*
If data is not avaiable on both socket, return zero.
If out of band data is avaiable on socket, return 1.
If data only avaiable on data socket, return 2.
On error, return -1.
*/
int is_oob(struct oob_sock *obs);

/*
Create 128 bits key. Key argument MUST is least 128bits size. If not
access error may be occur.
*/
int mk_key(char *key);

#endif
