/*
SYNOPSIS

    Solution for exercise 45-02.

REQUIREMENT

    Implement ftok()

SOLUTION

    IMPLEMENT ftok()

    Retrieve file stat
    Combine bits to make 32 bits IPC identity

        - 16 bits of i-node
        - 08 bits of dev identity
        - 08 high-bits of proj 

    TRY ftok()
     
        - Create template file by mktemp()
        - Call ftok() with above template file
        - Print IPC identity returned
        - Remove template file

USAGE
    
    $ ./dest/bin/45-02
    ipc-id: 1677787221

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

#include <tlpiexer/error.h>

key_t ftok(const char *file, int proj);

int main(int argc, char *argv[])
{
    char tmp_file[] = "/tmp/45-2-XXXXXX";
    int fd;
    int proj = 100;
    key_t ipc_id;

    // create template file
    errno = 0;
    fd = mkstemp(tmp_file);
    if (fd < 0)
        exit_err("mkstemp");
    close(fd);

    // create IPC identity
    if ((ipc_id = ftok(tmp_file, proj)) == -1)
        exit_err("ftok");

    // print IPC identity
    printf("ipc-id: %i\n", ipc_id);

    // remove template file
    if (unlink(tmp_file) == -1)
       exit_err("unlink");

    return 0;
}

key_t ftok(const char *file, int proj)
{
    key_t key;
    struct stat fstat;
    uint8_t *p;

    // retrieve file stat
    if (stat(file, &fstat) == -1)
        return -1;

    // combine bits
    p = (uint8_t*)&key;
    *(uint16_t*)p = (uint16_t)fstat.st_ino;
    *(uint8_t*)(p + 2) = (uint8_t)fstat.st_dev;
    *(uint8_t*)(p + 3) = *(uint8_t*)&proj;

    return key;
}
