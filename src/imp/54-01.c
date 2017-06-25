/*
SYNOPSIS

    Solution for exercise 54-01.

REQUIREMENT

    Rewrite the programs in Example 48-2 (svshm_xfr_writer.c) and Example 48-3
    (svshm_xfr_reader.c) to use POSIX shared memory objects instead of
    System V shared memory.

SOLUTION

    Use work flow same as

        tlpi/svshm/svshm_xfr.h
        tlpi/svshm/svshm_writer.c
        tlpi/svshm/svshm_reader.c

    Use POSIX semaphores and POSIX shared memory insted of System V semaphores
    and System V shared memory.

    Put shared information between writer and reader into
    "include/tlpiexer/54-01.h".

    WRITER

        Create semaphores sem_read with initial value is 0, sem_write with
        initial value is one.

        Create shared memory sh_mem with size is 1024 bytes.

        Enter loop to read data from stdin then send it to reader. Each loop
        performs actions below.

            Lock sem_read, so reader must wait to read data.

            Read data from stdin, put it to sh_mem and set size of read data.

            Unlock sem_write, so reader can lock it until reading data was
            done.

            If stdin is end of file, break loop.

        Lock sem_read, so reading data was done.

        Close sem_read, sem_write and sh_mem. Unlink all of it.

    READER

        Open semaphores sem_read, sem_write.

        Open shared memory sh_mem.

        Enter loop to read data from sh_mem then print it to stdout. Each
        loop performs actions below.

            Lock sem_write, so writer must wait until reading was done.

            Read data from sh_mem, write it to stdout.

            Unlock sem_read to allow reader perform writing loop.

            If sh_mem.size is zero, break loop.


        Close sem_read, sem_write, sh_mem.

USAGE

    # create file called "book.txt" with size=1024 * 8 = 8192 bytes
    $ dd if=/dev/zero of=tmp/book.txt bs=1024 count=8

    # start write service in background
    $ ./dest/bin/54-01-writer < cat tmp/book.txt &

    # start read service
    ./dest/bin/54-01-reader > tmp/book-copy.txt

    # checkout difference between book.txt and book-copy.txt
    # it won't print anythings, that mean two files are same
    diff tmp/book.txt tmp/book-copy.txt

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
