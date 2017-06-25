#include <tlpiexer/61-06.h>

#include <stdlib.h>

#include <tlpiexer/random.h>

int mk_key(char *key)
{
    size_t i;
    size_t n;

    for (i = 0; i < 16; i++) {
        if (rrand(0, 255, &n) != 0)
            return -1;
        key[i] = n;
    }
    return 0;
}

void mk_oob_sock(int data_sock, int oob_sock, struct oob_sock *obs)
{
    obs->fds[0].fd = data_sock;
    obs->fds[0].events = POLLIN;
    obs->fds[1].fd = oob_sock;
    obs->fds[1].events = POLLIN;
}

int is_oob(struct oob_sock *obs)
{
    int n;

    obs->fds[0].revents = 0;
    obs->fds[1].revents = 0;
    n = poll(obs->fds, 2, -1);
    if (n == -1)
        return -1;
    if (obs->fds[1].revents & POLLIN)
        return 1;
    else if (obs->fds[0].revents & POLLIN)
        return 2;
    else
        return 0;
}
