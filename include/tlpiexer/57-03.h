#ifndef __TLPIEXER_57_3_H__
#define __TLPIEXER_57_3_H__

#define SERVER_SOCK_PATH "/TLPIEXER-57-3-server"
#define CLIENT_SOCK_PATH "/TLPIEXER-57-3-client"
#define SOCK_BACKLOG 8

struct request
{
    size_t padding; 
};

struct response
{
    size_t seq_num;
};

#endif
