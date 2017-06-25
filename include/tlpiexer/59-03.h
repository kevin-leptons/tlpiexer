/*
SYNOPSIS

    usock_connect(const char *host, int type)
    usock_listen(const char *host, int type, int backlog)

DESCRIPTION

    usock_connect() create an UNIX domain socket, bind it to random path,
    connect it's socket to host then return local file descriptor.

    usock_listen() create an UNIX domain socket, bind it to host and
    call listen() on it then return local file descriptor.

    Both usock_connect() and usock_listen() use linux abstract path, so
    programmer doesn't remove file in host argument after that.

ARGUMENTS

    type argument is one of SOCK_DGRAM or SOCK_STREAM.

    backlog is number of connections hold by kernel when program process
    for on connection.

RETURN

    On success return possitive number as file descriptor. On error return
    -1 and errno set to non-zero.

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#ifndef __TLPIEXER_59_3_H__
#define __TLPIEXER_59_3_H__

#define USOCK_SERVER_PATH "/TLPIEXER-59-3-server"
#define BACKLOG 8

int usock_connect(const char *host, int type);
int usock_listen(const char *host, int type, int backlog);

#endif
