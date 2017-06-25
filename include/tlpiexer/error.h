#ifndef __TLPIEXER_ERROR_H__
#define __TLPIEXER_ERROR_H__

#define FN_OK 0
#define FN_ER -1

void exit_err(const char *msg);
void exit_usage(const char *msg);
void msg_err(const char *msg);

#endif
