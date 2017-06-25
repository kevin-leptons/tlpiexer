/*
SYNOPSIS

    Solution for exercise 05-07.
    
REQUIREMENT

    Implement readv() and writev() using read(), write(), and suiable
    functions from the malloc package (section 7.1.2).

SOLUTION

    Implement readv()

        - loop read data from file into vector with specify size

    Implement writev()

        - loop write data from vector into file with specify size

    Implement main()

        - parse options
        - verify arguments, options
        - switch to use_readv() or use_writev()

USAGE

    # create sample file
    $ echo {0..128} > tmp/data.txt

    # read data from file to vector
    $ ./dest/bin/05-07 read tmp/data.txt
    Read file's data to 3 vecter with size: 8, 16, 32 bytes
    vector 0: 0 1 2 3 
    vector 1: 4 5 6 7 8 9 10 1
    vector 2: 1 12 13 14 15 16 17 18 19 20 

    # write vector data to file
    $ ./dest/bin/05-07 write tmp/data.txt
    $ cat tmp/data.txt
    data-in-vector-0
    data-in-vector-1
    data-in-vector-2

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/uio.h>
#include <sys/stat.h>

#include <tlpiexer/error.h>

void showhelp();

// see "man 2 readv"
ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
ssize_t writev(int fd, const struct iovec *iov, int iovcnt);

int use_readv(char *file);
int use_writev(char *file);

int main(int argc, char **argv)
{
    // variables
    int opt;

    // parse options
    while ((opt = getopt(argc, argv, "h")) != -1) {
        switch (opt) {
            case 'h':
                showhelp();
                return EXIT_SUCCESS;
            default:
                showhelp();
                return EXIT_FAILURE;
        }
    }

    // verify arguments
    if (argc != 3) {
        showhelp();
        return EXIT_FAILURE;
    }

    // switch use_readv() or use_writev()
    if (strcmp(argv[1], "read") == 0)
        return use_readv(argv[2]);
    else if (strcmp(argv[1], "write") == 0)
        return use_writev(argv[2]);
    else {
        showhelp();
        return EXIT_FAILURE;
    }
}

int use_readv(char *file)
{
    // variables
    int fd;
    struct iovec iov[3];
    int i;

    printf("Read file's data to 3 vecter with size: 8, 16, 32 bytes\n");

    // setup io vector
    iov[0].iov_len = 8;
    iov[0].iov_base = malloc(8);
    iov[1].iov_len = 16;
    iov[1].iov_base = malloc(16);
    iov[2].iov_len = 32;
    iov[2].iov_base = malloc(32);

    // open
    fd = open(file, O_RDONLY);
    if (fd == -1)
        return EXIT_FAILURE;
    
    // read into vector
    if (readv(fd, iov, sizeof(iov)/sizeof(struct iovec)) == -1)
        return EXIT_FAILURE;

    // close
    if (close(fd) == -1)
        return EXIT_FAILURE;

    // display vector
    for (i = 0; i < sizeof(iov)/sizeof(struct iovec); i++) {
        printf("vector %i: %.*s\n", i, 
                (int)(iov + i)->iov_len, (char*)(iov + i)->iov_base);
    }

    // success
    return EXIT_SUCCESS;
}

ssize_t readv(int fd, const struct iovec *iov, int iovcnt)
{
    // variables
    int i;
    ssize_t rsize;
    ssize_t trsize = 0;

    // read for each element
    for (i = 0; i < iovcnt; i++) {
        rsize = read(fd, (iov + i)->iov_base, (iov + i)->iov_len);
        if (rsize == -1) {
            return -1;
        }
        trsize += rsize;
    }

    // success
    return trsize;
}

int use_writev(char *file)
{
    // variables
    int fd;
    struct iovec iov[3];
    char vdata0[] = "data-in-vector-0\n";
    char vdata1[] = "data-in-vector-1\n";
    char vdata2[] = "data-in-vector-2\n";

    printf("Write data from 3 vecter with size 8, 16, 32 bytes to file\n");

    // setup io vector
    iov[0].iov_base = (void*)vdata0;
    iov[0].iov_len = sizeof(vdata0);
    iov[1].iov_base = (void*)vdata1;
    iov[1].iov_len = sizeof(vdata1);
    iov[2].iov_base = (void*)vdata2;
    iov[2].iov_len = sizeof(vdata2);

    // open
    fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1)
        return EXIT_FAILURE;

    // write
    if (writev(fd, iov, sizeof(iov)/sizeof(struct iovec)) == -1)
        return EXIT_FAILURE;

    // success
    return EXIT_SUCCESS;
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt)
{
    // variables
    int i;
    ssize_t wsize;
    ssize_t twsize = 0;

    // write to fd for each element
    for (i = 0; i < iovcnt; i++) {
        wsize = write(fd, (iov + i)->iov_base, (iov + i)->iov_len);
        if (wsize == -1)
            return -1;

        twsize += wsize;
    }

    // success
    return twsize;
}

void showhelp()
{
    // variables
    char exename[] = "5-7";

    printf("\nUSAGE:\n\n");
    printf("%s read file\n", exename);
    printf("%s write file\n", exename);
    printf("%s -h\n\n", exename);

    printf("ARGUMENTS:\n\n");
    printf("read: read data from file use readv()\n"); 
    printf("write: write data to file use writev()\n"); 
    printf("file: file to read or write\n\n"); 
}
