/*
SYNOPSIS

    Solution for exercise 05-04.

REQUIREMENT

    Implement dup() and dup2() using fcntl() and, where neccessary, close()
    (You may ignore the fact that dup() and fcntl() return different errno
    values for some error case). For dup2(), remember to handle the special
    case where oldfd equals newfd. In this case, you should check wheter
    oldfd is valid, which can be done by, for example, check if fcntl*oldfd,
    F_GETFL) succeeds. If oldfd is not valid, then the function should 
    return -1 with errno set to EBADF

SOLUTION

    Implement dup()

        - call fcntl(oldfd, F_DUPFD, 0)
        - return file description from fcntl()

    Implement dup2()

        - verify oldfd, newfd
        - call fcntl(oldfd, F_DUPFD, newfd)
        - return file description from fcntl()
 
    Implement main()

        - try use dup()
        - try use dup2()

USAGE

    $ ./dest/bin/05-04 tmp/fd.txt
    $ cat tmp/fd.txt
    few words from fdx
    few words from fdy

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include <tlpiexer/error.h>

void showhelp();

// see "man dup 2"
int dup(int oldfd);
int dup2(int oldfd, int newfd);

int main(int argc, char **argv)
{
    // variables
    int fdx, fdy;
    char fdx_data[] = "few words from fdx\n";
    char fdy_data[] = "few words from fdy\n";

    // open file into fdx
    fdx = open(argv[1], O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
    if (fdx == -1)
        exit_err("open:fdx");
    printf("open: fdx=%i\n", fdx);

    // duplicate file-open-table of fdx to fdy
    fdy = dup2(fdx, 5);
    if (fdy == -1)
        exit_err("dup2");
    printf("dup2 fdx=>fdy: fdy=%i\n", fdy);

    // write to fdx
    if (write(fdx, fdx_data, sizeof(fdx_data)) == -1)
        exit_err("write:fdx");
    printf("write to fdx: %s", fdx_data);

    // write to fdy
    if (write(fdy, fdy_data, sizeof(fdy_data)) == -1)
        exit_err("write:fdy");
    printf("write to fdy: %s", fdy_data);

    // close file descriptors
    if (close(fdx) == -1)
        exit_err("close:fdx");
    if (close(fdy) == -1)
        exit_err("close:fdy");

    // success
    return EXIT_SUCCESS;
}

int dup(int oldfd)
{
    // variables
    int newfd;

    // duplicate
    newfd = fcntl(oldfd, F_DUPFD, 0);

    // non-negative on success
    // -1  on error
    return newfd;
}

int dup2(int oldfd, int newfd)
{
    // variables
    int dupfd;

    // verify oldfd
    if (fcntl(oldfd, F_GETFD) == -1) {
        errno = EBADF;
        return -1;
    }

    // verify oldfd, newfd
    if (oldfd == newfd)
        return newfd;

    // close
    close(newfd);

    // duplicate
    dupfd = fcntl(oldfd, F_DUPFD, newfd);

    // non-negative, that is newfd on success
    // -1 on error
    return dupfd;
}

void showhelp()
{
    // variables
    char exename[] = "5-3";

    printf("\nUSAGE:\n\n");
    printf("%s\n", exename);
    printf("%s -h\n\n", exename);
}
