#ifndef __TLPIEXER_52_2_H__
#define __TLPIEXER_52_2_H__

#define MQ_PATH_52_2 "/tlpiexer-52-02"
#define MQ_PATH_52_2_CLIENT_TMP "/tlpiexer-52-02"

typedef struct {
    int mq_name_size;
    char mq_name[255];
} request_t;

typedef struct {
    int seq_num;
} response_t;

#endif
