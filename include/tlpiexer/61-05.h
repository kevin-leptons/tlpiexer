/*
SPEC

    Part of solution of exercise 61-5. See "src/imp/61-5" for more
    information.

AUTHOR

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#ifndef __TLPIEXER_61_5_H__
#define __TLPIEXER_61_5_H__

#define USRNAME_MAX 32
#define PASSWD_MAX 32
#define CMD_MAX 1024
#define SERVER_PORT "5001"
#define BACKLOG 8
#define BUF_SIZE 1024

struct msg_req
{
    char usr[USRNAME_MAX];
    char pwd[PASSWD_MAX];
    char cmd[CMD_MAX];
};

int signin(const char *usrname, const char *pass);

#endif
