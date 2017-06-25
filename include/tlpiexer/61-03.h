/*
SPEC

    Different interface for sendfile() called sendfile_x().

AUTHOR

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#ifndef __TLPIEXER_61_3_H__
#define __TLPIEXER_61_3_H__

ssize_t sendfile_x(int out_fd, int in_fd, off_t *offset, size_t count);

#endif
