#include <tlpiexer/59-01.h>

#include <unistd.h>
#include <string.h>
#include <tlpiexer/error.h>

int init_lbuf(int fd, struct lbuf *lbuf)
{
    size_t rsize;

    rsize = read(fd, lbuf->buf, sizeof(lbuf->buf));
    if (rsize == FN_ER)
        return FN_ER;

    lbuf->ebuf = lbuf->buf + sizeof(lbuf->buf);
    lbuf->fd = fd;
    lbuf->cur = lbuf->buf;
    lbuf->efile = rsize < sizeof(lbuf->buf);

    return FN_OK;
}

int read_lbuf(struct lbuf *lbuf, char *line, size_t size)
{
    char *seek = lbuf->cur;
    size_t rsize;
    size_t tsize;
    size_t lsize = 0;

    for (;;) {
        if (seek == lbuf->ebuf) {
            // treat end-of-file as new-line 
            if (lbuf->efile) 
                break;

            // move rest of data to begin of buffer
            tsize = lbuf->ebuf - lbuf->cur;
            memcpy(lbuf->buf, lbuf->cur, tsize);

            // read more from file and put into next of rest data
            rsize = read(lbuf->fd, lbuf->buf + tsize, 
                         sizeof(lbuf->buf) - tsize);
            if (rsize == 0) {
                lbuf->efile = true;
                return 0;
            } else if (rsize == FN_ER) {
                return FN_ER;
            }

            // reset arguments
            lbuf->cur = lbuf->buf;
            seek = lbuf->buf + tsize;
        }

        if (*seek == '\n')
            break;
        lsize++;
        seek++;

        // out of line buffer
        if (lsize > size)
            return FN_ER;
    }

    // copy data from buffer to output line
    memcpy(line, lbuf->cur, seek - lbuf->cur);
    lbuf->cur = seek + 1;

    return lsize;
}
