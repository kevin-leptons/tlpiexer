/*
SYNOPSIS

    Part of solution for excercise 59-04. See 59-04.c for more information.

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>

#include <tlpiexer/error.h>
#include <tlpiexer/59-04.h>
#include <tlpiexer/59-04-isock.h>

static const char *lerr_str[6] = {
    "Success",
    "Permision denied",
    "Item is early exist",
    "Item doesn't exist",
    "Internal server error",
    "Unknow command"
};

static void _exit_usage(FILE *stream);
static const char *lstr_err(int lerrno);

int main(int argc, char *argv[])
{
    int lfd;
    struct msg_req req;
    struct msg_res res;
    struct passwd *pwd;

    // prepare request
    if (argc < 3)
        _exit_usage(stderr);
    pwd = getpwuid(getuid());
    if (pwd == NULL)
        exit_err("getpwuid");
    strncpy(req.item.own, pwd->pw_name, sizeof(req.item.own));
    strncpy(req.item.name, argv[2], sizeof(req.item.name));
    if (strcmp(argv[1], "add") == 0) {
        if (argc != 4)
            _exit_usage(stderr);
        req.cmd = REQ_ADD;
    } else if (strcmp(argv[1], "del") == 0) {
        if (argc != 3)
            _exit_usage(stderr);
        req.cmd = REQ_DEL;
    } else if (strcmp(argv[1], "get") == 0) {
        if (argc != 3)
            _exit_usage(stderr);
        req.cmd = REQ_GET;
    } else if (strcmp(argv[1], "set") == 0) {
        if (argc != 4)
            _exit_usage(stderr);
        req.cmd = REQ_SET;
    } else {
        _exit_usage(stderr);
    }
    if (argc == 4)
        strncpy(req.item.value, argv[3], sizeof(req.item.value));

    // connect to server
    lfd = isock_connect("localhost", SERVER_PORT, SOCK_STREAM);
    if (lfd == FN_ER)
        exit_err("isock_connect");

    // write request
    if (write(lfd, &req, sizeof(req)) != sizeof(res))
        exit_err("write");

    // read response
    if (read(lfd, &res, sizeof(res)) != sizeof(res))
        exit_err("read");

    // display result
    printf("%s\n", lstr_err(res.result));
    if (res.result == RES_OK) {
        if (req.cmd == REQ_GET)
            printf("name=%s; value=%s; own=%s\n", 
                    res.item.name, res.item.value, res.item.own);
    }

    close(lfd);
    return EXIT_SUCCESS;
}

static void _exit_usage(FILE *stream)
{
    fprintf(stream, "USAGE:\n"
                "\tcmd add <name> <value>\n"
                "\tcmd get <name>\n"
                "\tcmd set <name> <value>\n"
                "\tcmd del <name>\n");
    if (stream == stdout)
        exit(EXIT_SUCCESS);
    else
        exit(EXIT_FAILURE);
}

static const char *lstr_err(int lerrno)
{
    if (lerrno >= sizeof(lerr_str))
        return NULL;
    return lerr_str[lerrno];
}
