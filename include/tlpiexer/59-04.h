/*
SPEC

    Shared information between server and client of excercise 49-4.
    See "src/imp/59-4.c" for more information.

AUTHOR

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#ifndef __TLPIEXER_59_4_H__
#define __TLPIEXER_59_4_H__

#include <tlpiexer/59-04-llist.h>

#define SERVER_PORT "9000"
#define BACKLOG 8

enum req_cmd
{
    REQ_ADD = 1,
    REQ_GET = 2,
    REQ_SET = 3,
    REQ_DEL = 4
};

enum res_res
{
    RES_OK = 0,
    RES_PERM = 1,
    RES_EXIST = 2,
    RES_NOEXT = 3,
    RES_EXCEPT = 4,
    RES_UNKOW = 5
};

struct msg_req
{
    enum req_cmd cmd;
    struct item_nv item;
};

struct msg_res
{
    enum res_res result;
    struct item_nv item;
};

#endif
