#ifndef __TLPIEXER_52_3_H__
#define __TLPIEXER_52_3_H__

#define MQ_SERVER "/TLPIEXER-mq-52-3"
#define MQ_CLIENT_TMP "/TLPIEXER-mq-52-3-XXXXXX"

#define PATH_MAX 255
#define BUF_SIZE 1024

typedef struct {
    char mq_client[PATH_MAX];
    char file[PATH_MAX];
} request_t;

typedef enum {
    END = 0,
    PART = 1,
    ERROR = 2
} mstate_t;

typedef struct {
    mstate_t state;
    int size;
    char data[BUF_SIZE];
} response_t;

#endif
