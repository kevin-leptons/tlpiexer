/*
SYNOPSIS

    Solution for exercise 18-08.

REQUIRE

    Implement nftw() (This will rqeuire the use of the opendir(), readdir(),
    closedir(), and stat() system calls, among others).

SOLUTION

    Implement nftw()

        - call fwalk() with arguments

    Implement fwalk()

        - call opendir(), readir() to inspect file
        - with file, invoke callback funtion
        - with directory, call fwalk() recursive

    Implement main()

        - parse options
        - verify argument, options
        - switch to call nftw() with arguments

USAGE

    $ ./dest/bin/18-07 
 CMakeLists.txt
 lib
    59-4-llist.c
    59-4-isock.c
    61-5.c
    61-3.c
    59-1.c
    exit-error.c
    ...
 imp
    59-4-server.c
    28-1.c
    34-7.c
    31-2.c
    18-9.c
    13-01.c
    ...

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

#include <tlpiexer/error.h>

#define RET_SUCCESS 0
#define RET_FAILURE -1

// ntfw flags
#define FTW_PHYS 1
#define FTW_MOUNT 2
#define FTW_CHDIR 4
#define FTW_DEPTH 8

// spec     : walk tree information
struct FTW {
    // offset of base name with file path
    int base;

    // level of depth
    int level;
};

// spec     : callback function use for nftw
// arg      :
//  - fpath: file path, it is relative path with current directory
//  - status: status of file
//  - type: type flag
//  - ftw: contains level of file from walk directory
// ret      :
//  - 0 on success, nftw will continue
//  - other on error, nftw will stop
typedef 
int (*nftw_callback) (const char *fpath, const struct stat *status, int type, 
        struct FTW *ftw);

// spec     : walk though directory rescusive and call function
// arg      : 
//  - dirpath: path to directory will be walk though
//  - callback: function will be call on each entry of directory
//  - nfd: max number of file descriptor will be use
//  - flags: flags use to walk though directory
//      - FTW_CHDIR: change current directory when walk
//      - FTW_DEPTH: report all file in directory before it self directory
//      - FTW_MOUNT: report only files in same file system
//      - FTW_PHYS: not follow symbol link
// ret      :
//  - 0 on success
//  - -1 on error
int nftw(const char *dirpath, nftw_callback callback, int nfd, int flags);

// spec     : walk deep from path and call callback function
// arg      :
//  - path: path to directory
//  - callback: call back function will be call for each sub file and directory
//  - nfd: max of number of file description allow open
//  - flags: flags use to walk through directory
//  - level: level call this function, base on 0
int fwalk(const char *dirpath, nftw_callback callback, int nfd, int flags, 
        int level);

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// sample of callback use for nftw
int 
entrycb(const char *path, const struct stat *status, int type, struct FTW *ftw);

int main(int argc, char **argv)
{
    // variables
    int opt;
    char *dpath;
    int flags;

    // init
    flags = 0;
    
    // parse options
    while((opt = getopt(argc, argv, "hdpmc")) != -1) {
        switch (opt) {
            case 'c':
                flags |= FTW_CHDIR;
                break;
            case 'd':
                flags |= FTW_DEPTH;
                break;
            case 'p':
                flags |= FTW_PHYS;
                break;
            case 'm':
                flags |= FTW_MOUNT;
                break;
            case 'h':
                showhelp();
                return EXIT_SUCCESS;
            default:
                showhelp();
                return EXIT_FAILURE;
        }
    }

    // verify dirpath
    if (optind >= argc || argc == 1)
        dpath = (char*)&".";
    else
        dpath = argv[optind];
    
    if (nftw(dpath, entrycb, 8, flags) == -1)
        exit_err("nftw");

    // success
    return  EXIT_SUCCESS;
}

int nftw(const char *dirpath, nftw_callback callback, int nfd, int flags)
{
    return fwalk(dirpath, callback, nfd, flags, 0);
}

int fwalk(const char *dirpath, nftw_callback callback, int nfd, int flags, 
        int level)
{
    // variables
    DIR *dir;
    struct dirent *de;
    char fpath[PATH_MAX];
    char lpath[PATH_MAX];
    struct stat status, dstatus;
    struct FTW ftw;
    char cwd[PATH_MAX];

    // verify nfd
    if (nfd < 0)
        return RET_FAILURE;
    
    // init
    ftw.level = level;
    ftw.base = 0;

    // device id
    if ((flags & FTW_MOUNT) == FTW_MOUNT) {
        if (stat(dirpath, &dstatus) == -1)
            return RET_FAILURE;
    } 

    // open
    if ((dir = opendir(dirpath)) == NULL)
        return RET_FAILURE;

    // change working dir
    if (getcwd(cwd, PATH_MAX) == NULL)
        return RET_FAILURE;
    if ((flags & FTW_CHDIR) == FTW_CHDIR)
        if (chdir(dirpath) == -1)
            return RET_FAILURE;

    // walk on directory and call callback function
    errno = 0;
    while ((de = readdir(dir)) != NULL) {

        // skip for current, up directory
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;

        // build file path
        if ((flags & FTW_CHDIR) == FTW_CHDIR) {
            strcpy(fpath, de->d_name);
        } else {
            strcpy(fpath, dirpath);
            strcat(fpath, "/");
            strcat(fpath, de->d_name);
        }

        // resolve link
        if (de->d_type == DT_LNK) {
            if ((flags & FTW_PHYS) == 0) {
                if (realpath(fpath, lpath) == NULL)
                    return RET_FAILURE;
                strcpy(fpath, lpath);
            }
        }

        // status of file
        if (lstat(fpath, &status) != 0)
            return RET_FAILURE;

        // verify device
        if (((flags & FTW_MOUNT) == FTW_MOUNT) && 
                status.st_dev != dstatus.st_dev)
            continue;

        // callback for files in directory
        if (!S_ISDIR(status.st_mode)) {
            if (callback(de->d_name, &status, de->d_type, &ftw) != 0)
                return RET_FAILURE;
        } else {
        // walk on deeper directory

            // report this directory before report contents of directory
            if ((flags & FTW_DEPTH) != FTW_DEPTH) {
                if (callback(de->d_name, &status, status.st_mode, &ftw) != 0)
                    return RET_FAILURE;
            }

            if (fwalk(fpath, callback, nfd - 1, flags, level + 1) != 0)
                return RET_FAILURE;

            // report this directory after report contents of directory
            if ((flags & FTW_DEPTH) == FTW_DEPTH) {
                if (callback(de->d_name, &status, status.st_mode, &ftw) != 0)
                    return RET_FAILURE;
            }
        }
    } 
    if (errno != 0)
        return RET_FAILURE;

    // close
    if (closedir(dir) == -1)
        return RET_FAILURE;

    // up directory
    if ((flags & FTW_CHDIR) == FTW_CHDIR)
        if (chdir(cwd) == -1)
            return RET_FAILURE;
    
    // success
    return RET_SUCCESS;
}

int 
entrycb(const char *path, const struct stat *status, int type, struct FTW *ftw)
{
    printf("%*s%s\n", 4*ftw->level, " ", path);
    return RET_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "18-8";

    printf("\nUSAGE:\n\n");
    printf("\t%s [-c] [-m] [-p] [-d] [dir]\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("ARGUMENTS\n\n");
    printf("\tdir: path to directory to try nftw()\n\n");

    printf("OPTIONS:\n\n");
    printf("\t-c: call nftw() with FTW_CHDIR\n");
    printf("\t-m: call nftw() with FTW_MOUNT\n");
    printf("\t-p: call nftw() with FTW_PHYS\n");
    printf("\t-d: call nftw() with FTW_CHDIR\n\n");
}
