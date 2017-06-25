/*
SYNOPSIS

    Solution for exercise 16-07.

REQUIREMENT

    Write a simple version of the chattr(1) command, which modifies file
    i-node flags. See the chattr(1) man page for details of he chattr
    command-line interface (You don't need to implement the -R, -V, and -v
    options).

SOLUTION

    - parse options
    - verify arguments, options
    - open file
    - get attributes of file by ioctl()
    - parse mode to create file attribute
    - set file attributes by ioctl()
    - close file

USAGE

    $ touch tmp/f.txt
    $ lsattr tmp/f.txt
    -------------e-- tmp/f.txt

    $ ./dest/bin/15-07 +uA tmp/f.txt
    $ lsattr tmp/f.txt
    -u-----A-----e-- tmp/f.txt

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/fs.h>
#include <sys/ioctl.h>

#include <tlpiexer/error.h>

#define RET_SUCCESS 0
#define RET_FAILURE -1
#define ATTRSET "acDijAdtsSTu"

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : parse mode into attributes
// arg      :
//  - mode: string contains character attributes
//  - attr: output, attribute
// ret      :
//  - 0 on success
//  - -1 on error
int pattr(char *mode, int *attr);

int main(int argc, char **argv)
{
    // variables
    int oattr, nattr;
    int fd;

    // init
    oattr = 0;
    nattr = 0;

    // verify arguments
    if (argc != 3) {
        showhelp();
        return EXIT_FAILURE;
    }

    // open
    fd = open(argv[2], O_RDONLY);
    if (fd == -1)
        exit_err("open");

    // reteview attributes
    if (ioctl(fd, FS_IOC_GETFLAGS, &oattr) == -1)
        exit_err("ioctl:FS_IOC_GETFLAGS");

    // parse mode
    if (pattr(argv[1] + 1, &nattr) == -1)
        exit_err("pattr");

    // joind attributes
    switch (argv[1][0]) {
        case '+':
            nattr = oattr | nattr;
            break;
        case '-':
            nattr = oattr & ~nattr;
            break;
        case '=':
            // not doing here
            // because only attributes has parsed put into nattr
            break;
        default:
            fprintf(stderr, "unknow operation: %c\n", argv[1][0]);
            return EXIT_FAILURE;
    }

    // set mode
    if (ioctl(fd, FS_IOC_SETFLAGS, &nattr) == -1)
        exit_err("ioctl:FS_IOC_SETFLAGS");

    // close
    if (close(fd) == -1)
        exit_err("close");

    // success
    return EXIT_SUCCESS;
}

int pattr(char *mode, int *attr)
{
    // verify each character of mode
    while (*mode != 0) {
        switch (*mode) {
            case 'a':
                *attr |= FS_APPEND_FL;
                break;
            case 'c':
                *attr |= FS_COMPR_FL;
                break;
            case 'D':
                *attr |= FS_DIRSYNC_FL;
                break;
            case 'i':
                *attr |= FS_IMMUTABLE_FL;
                break;
            case 'j':
                *attr |= FS_JOURNAL_DATA_FL;
                break;
            case 'A':
                *attr |= FS_NOATIME_FL;
                break;
            case 'd':
                *attr |= FS_NODUMP_FL;
                break;
            case 't':
                *attr |= FS_NOTAIL_FL;
                break;
            case 's':
                *attr |= FS_SECRM_FL;
                break;
            case 'S':
                *attr |= FS_SYNC_FL;
                break;
            case 'T':
                *attr |= FS_TOPDIR_FL;
                break;
            case 'u':
                *attr |= FS_UNRM_FL;
                break;
            default:
                return RET_FAILURE;
        }

        mode += 1;
    }

    // success
    return RET_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "15-7";

    printf("\nUSAGE:\n\n");
    printf("%s [mode] file\n", exename);
    printf("%s -h\n\n", exename);

    printf("ARGUMENTS:\n\n");
    printf("file: file to change attribute\n\n");

    printf("OPTIONS:\n\n");
    printf("mode: [+-=][a, c, D, i, j, A, d, t, s, S, T, u]\n\n"); 
}
