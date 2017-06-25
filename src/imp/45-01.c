/*
SYNOPSIS

    Solution for exercise 45-01.

REQUIREMENT

    Write a program to verify that the algorithm employed by ftok() uses the
    file's i-node number, minor device number, and proj value, as described
    in Section 45.2 (It is sufficient to print all of these values, as well
    as the return value from ftok(), in hexadecimal, and inspect the results
    for a few examples).

SOLUTION

    Create an template file

        - Create an template file with mktemp()

    Create an IPC key

        - Create IPC key by ftok() with template file above
        - Print 32 bits IPC key in heximal format

    Inspect information of template file, proj

        - Retrieve file information with stat()
        - Print i-node, device number and proj i heximal format

    Remove template file

USAGE

    $ ./dest/bin/45-01
    ipc-id      : 0x55/00/01/64
    i-node/dev/proj : 0x55/00/01/64

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

#include <tlpiexer/error.h>

int main(int argc, char *argv[])
{
    char tmp_file[] = "/tmp/tlpi-45-1-XXXXXX";
    key_t ipc_id;
    int proj = 100;
    struct stat fstat;
    int fd;
    uint8_t *p;


    // create template file
    errno = 0;
    fd = mkstemp(tmp_file);
    if (fd < 0)
        exit_err("mkstemp");
    close(fd);

    // create IPC identity
    ipc_id = ftok(tmp_file, proj);
    if (ipc_id == -1)
       exit_err("ftok");

    // print IPC identity 
    p = (uint8_t*)&ipc_id;
    printf("ipc-id\t\t: 0x%02x/%02x/%02x/%02x\n", 
            *(p + 0), *(p + 1), *(p + 2), *(p +3));

    // retrieve file stat
    if (stat(tmp_file, &fstat) == -1)
       exit_err("stat");
    
   // print i-node, device number of file and proj
   p = (uint8_t*)&fstat.st_ino;
   printf("i-node/dev/proj\t: 0x%02x/%02x/%02x/%02x\n", 
           *(p + 0), *(p + 1), *(uint8_t*)&fstat.st_dev, *(uint8_t*)&proj);

   // remove template file
   if (unlink(tmp_file) == -1)
       exit_err("unlink");

    return 0;
}
