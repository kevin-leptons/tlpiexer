#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <tlpiexer/error.h>

#define BUF_SIZE 1024

ssize_t sendfile_x(int out_fd, int in_fd, off_t *offset, size_t count)
{
    ssize_t rsize;
    ssize_t wsize;
    ssize_t to_rsize;
    ssize_t total_wsize = 0;
    struct stat stat;
    char buf[BUF_SIZE];

    // verify in_fd, it must regular file
    if (fstat(in_fd, &stat) == FN_ER)
        return FN_ER;
    if (!S_ISREG(stat.st_mode))
        return FN_ER;

    // seek to specific offset
    if (offset != NULL) {
        if (lseek(in_fd, SEEK_SET, *offset) == FN_ER)
            return FN_ER;
    }

    // loop to transfer data
    for (;;) {
        // adjust to_rsize to fit with count
        if (total_wsize >= count)
            break;
        else if (count - total_wsize >= sizeof(buf))
            to_rsize = sizeof(buf);
        else
            to_rsize = count - total_wsize;

        // read block
        rsize = read(in_fd, buf, to_rsize);
        if (rsize == FN_ER)
            return FN_ER;
        if (rsize == 0)
            break;

        // write block
        wsize = write(out_fd, buf, rsize);
        if (wsize != rsize)
            return FN_ER;
        total_wsize += wsize;
    }

    // update offset
    if (offset != NULL) {
        *offset = lseek(in_fd, SEEK_CUR, 0);
        if (*offset == FN_ER)
            return FN_ER;
    }

    return total_wsize;
}
