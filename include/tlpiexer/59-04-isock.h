/*
NAME

    Internet socket domain library.

SYNOPSIS

    int isock_connect(const char *host, const char *service, int type);
    int isock_listen(const char *service, int type, int backlog);
    int isock_bind(const char *service, int type);

DESCRIPTION

    isock_connect() create an internet domain socket, connect that socket to 
    host-service. It return file descriptor.

    isock_listen() create an internet domain socket, bind it with service
    specification, call listen() on it. It return file descriptor.

    isock_bind() create an internet domain socket, bind it to service
    specification. It return file descriptor.

ARGUMENTS

    type must specific as SOCK_DGRAM or SOCK_STREAM.

RETURN

    isock_connect(), isock_listen() on success, return positive number as 
    file descriptor. 

    isock_bind() on success return FN_OK.
    
    On fail, return error number same as returned of getaddrinfo() if
    getaddrinfo() fail. If other fail, return FN_ER and errno set to 
    error number.

AUTHOR

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#ifndef __TLPIEXER_59_4_ISOCK_H__
#define __TLPIEXER_59_4_ISOCK_H__

int isock_connect(const char *host, const char *service, int type);
int isock_listen(const char *service, int type, int backlog);
int isock_bind(int fd, const char *service, int type);

#endif
