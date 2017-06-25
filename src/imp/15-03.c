/*
SYNOPSIS

    Solution for exercise 15-03.

REQUIREMENT

    On a system running linux 2.6, modify the program in listing 15-2
    (t_stat.c) so that the file timestamps are displayed with nanosecond
    acccuracy.

SOLUTION

    - copy source code from tlpi/files/t_stat.c
    - modify to meet requirements

USAGE

    # work in temporary directory because symbol is relative
    $ cd tmp
    $ touch f.txt
    $ ln -s f.txt l.txt 

    # retrive file's info
    $ ./dest/bin/15-03 f.txt
    file type:  regular file
    i-node: major=8, minor=17
    i-node number:  9453311
    mode:   100644
    number of hard link:    1
    owner:  uid=1000, gid=1000
    file size:  0 bytes
    optimal block size: 4096 bytes
    512B blocks allocated:  0
    last file access:   Mon Jun 12 18:06:57 2017
    :454775521
    last file modification: Mon Jun 12 17:59:37 2017
    :938764389
    last status change: Fri Oct  1 14:53:09 1999
    :938764389

    # retrieve link's info
    $ ./dest/bin/15-03 l.txt
    file type:  symbol link
    i-node: major=8, minor=17
    i-node number:  9453306
    mode:   120777
    number of hard link:    1
    owner:  uid=1000, gid=1000
    file size:  5 bytes
    optimal block size: 4096 bytes
    512B blocks allocated:  0
    last file access:   Mon Jun 12 18:06:43 2017
    :790775175
    last file modification: Mon Jun 12 18:06:36 2017
    :966775002
    last status change: Sun Aug 20 19:36:42 2000
    :966775002

AUTHORS

    Michael Kerrisk
    Kevin Leptons <kevin.leptons@gmail.com>
*/

#define _BSD_SOURCE

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <tlpiexer/error.h>

#define RET_SUCCESS 0
#define RET_FAILURE -1
#define ARR_MAX 999999

// spec     : show status of file
// arg      :
//  - s: structure contains status of file
// ret      : none
static void show_fstat(const struct stat *s);

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

int main(int argc, char **argv)
{
    // variables
    struct stat s;
    int ftype;
    char opt;

    // init
    ftype = 0;

    // parse arguments
    while ((opt = getopt(argc, argv, "hl")) != -1) {
        switch (opt) {
            case 'l':
                ftype = 1;
                break;
            case 'h':
                showhelp();
                return EXIT_SUCCESS;
            default:
                showhelp();
                return EXIT_FAILURE;
        }
    }

    // verify file name
    if (optind >= argc) {
        showhelp();
        return EXIT_FAILURE;
    }

    // poll file status
    switch (ftype) {
        case 0:
            if (stat(argv[optind], &s) == -1)
                exit_err("stat");
            break;
        case 1:
            if (lstat(argv[optind], &s) == -1)
                exit_err("lstat");
            break;
        default:
            showhelp();
            return EXIT_FAILURE;
    }

    // show
    show_fstat(&s);

    // success
    return EXIT_SUCCESS;
}

static void show_fstat(const struct stat *s)
{
    printf("file type:\t");
    switch (s->st_mode & S_IFMT) {
        case S_IFREG: printf("regular file\n"); break;
        case S_IFDIR: printf("directory\n"); break;
        case S_IFCHR: printf("character device\n"); break;
        case S_IFBLK: printf("block device\n"); break;
        case S_IFLNK: printf("symbol link\n"); break;
        case S_IFIFO: printf("FIFO or pipe\n"); break;
        case S_IFSOCK: printf("socket\n"); break;
        default: printf("unknow\n"); break;
    }

    printf("i-node:\tmajor=%ld, minor=%ld\n", 
            (long)major(s->st_dev), (long)minor(s->st_dev));

    printf("i-node number:\t%ld\n", (long)s->st_ino);

    printf("mode:\t%lo\n", (unsigned long)s->st_mode);

    printf("number of hard link:\t%ld\n", (long)s->st_nlink);

    printf("owner:\tuid=%ld, gid=%ld\n", (long)s->st_uid, (long)s->st_gid);

    if (S_ISCHR(s->st_mode) || S_ISBLK(s->st_mode))
        printf("device number:\t%major=%ld, minor=%ld\n",
                (long)major(s->st_rdev), (long)minor(s->st_rdev));

    printf("file size:\t%lld bytes\n", (long long)s->st_size);

    printf("optimal block size:\t%ld bytes\n", (long)s->st_blksize);

    printf("512B blocks allocated:\t%lld\n", (long long)s->st_blocks);

    printf("last file access:\t%s:%ld\n", 
            ctime(&s->st_atim.tv_sec), s->st_atim.tv_nsec);
    printf("last file modification:\t%s:%ld\n", 
            ctime(&s->st_mtim.tv_sec), s->st_mtim.tv_nsec);
    printf("last status change:\t%s:%ld\n", 
            ctime(&s->st_ctim.tv_nsec), s->st_ctim.tv_nsec);
}

void showhelp()
{
    // variables
    char exename[] = "15-3";

    printf("\nUSAGE:\n\n");
    printf("%s [-l] file\n", exename);
    printf("%s -h\n\n", exename);

    printf("OPTIONS:\n\n");
    printf("-l: show state of link file\n\n"); 

    printf("ARGUMENTS\n\n");
    printf("file: file to show state\n\n");
}
