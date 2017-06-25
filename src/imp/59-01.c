/*
SYNOPSIS

    Solution for excercise 59-01.

REQUIREMENT

    When reading large quantities of data, the readLine() function shown
    in Listing 59-1 is inefficient, since a system call is required to read
    each character. A more efficient interface would read block of characters
    into a buffer and extract a line at a time from this buffer. Such an
    interface might consist of two functions. The first of these functions,
    which might be called readLineBufInit(fd, &rlbuf), initializes the
    bookeeping data structure pointed to be by rbuf. This structure includes
    space for a data buffer, the size of that buffer, and a pointer to the
    next "unread" character in that buffer. It also includes a copy of the
    file descriptor given in the argument fd. The second function,
    readLineBuf(&rlbuf), returns the next line from the buffer associated
    with rlbuf. If required, this function reads a further block of data from
    the file descriptor saved in rlbuf. Implement these two functions. Modify
    the programs in Listing 59-6 (is_seqnum_sv.c) and Listing 59-7
    (is_seqnum_cl.c) to use these functions.

SOLUTION

    These two functions will be implement in "src/lib" and change to

        readLineBufInit(fd, *rlbuf)
        => init_lbuf(fd, struct lbuf *lbuf)

        readLine(*rlbuf)
        => read_lbuf(struct lbuf *buf, char *line, size_t len)

    is_seqnum_sv.c and is_seqnum_cl.c will be re-implement in respectively
    "src/imp/59-01-server.c" and "src/imp/59-01-client.c"

    IMPLEMENT init_rlbuf(fd, *lbuf)

        Read first block from file to buffer of lbuf.

        Set fields of lbuf to correct values.


    IMPLEMENT read_rlbf(*lbuf, char *line, size_t len)

        Seek end line character from current position. If found, copy data from
        buffer to line. If not found and seeker reach end of buffer, move rest
        of data into begin of buffer, read more data from file to buffer, then
        continue seek. If not found and buffer is out, return error.

USAGE

    # run server in background
    $ ./dest/bin/59-01-server &

    # invoke server few times
    $ ./dest/bin/59-01-client localhost
    server:connect from localhost:36606
    client:response:1

    $ ./dest/bin/59-01-client 3
    server:connect from localhost:36607
    cilent:response:4

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("Goto %s to see solution\n", __FILE__);
    return EXIT_SUCCESS;
}
