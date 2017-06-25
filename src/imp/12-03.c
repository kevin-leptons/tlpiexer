/*
SYNOPSIS

    Solution for exercise 12-03.

REQUIREMENT

    Write a program that lists all procsses that have a particular file
    pathname open. This can be achieved by inspecting the contents of all of 
    the /proc/pid/fd/ synbolic links. This will require nested loops employing
    readdir(3) to scan all /proc/pid directories, and then contents of all
    /proc/pid/fd entries within each /proc/pid directory. To read the contents
    of a /proc/pid/fd/n symbolic link requires the use of readlink(), described
    in section 18.5.

SOLUTION

    Same as hints of requirements.

USAGE

    # create an file and open it
    $ touch tmp/data.txt
    $ tail -f /dev/null > tmp/data.txt &
    [1] 21305

    # see what are process open it
    $ ./dest/bin/12-03 tmp/data.txt
    tail - 22816

    # stop open file
    kill %1

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/
 
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include <tlpiexer/error.h>

#define RET_SUCCESS 0
#define RET_FAILURE -1
#define PNAME_MAX 128
#define PSTATUS_MAX 4096
#define STATUS_MAX 64
#define PNAME_TAG "Name:"

// spec     : get max of number of file descriptor can be open in same time
// arg      : none
// ret      : number of file descriptor
size_t fdmax(void);

// spec     : get name of process from content of status file
// arg      :
//  - data: content of status file
//  - size: size of data
//  - name: name of process
// ret      :
//  - 0 on success
//  - -1 on error
int pnamefstatus(char *data, size_t size, char *name);

// spec     : verify file specify by path is opening by process id specify 
//            by spid
// arg      :
//  - spid: process id in string format
//  - path: path to file to verify
// ret      :
//  - 1 on file is opening by process
//  - 0 on file is not opening by process
//  - -1 on error
int fdfproc(char *spid, char *path);

// spec     : verify string is numberic
// arg      :
//  - str: string to verify
// ret      :
//  - non-zero on string is numberic
//  - 0 on string is not numberic
int isnumberic(const char *str);

int main(int argc, char **argv)
{
    // variables
    DIR *proc;
    struct dirent *subdir;
    char pname[PNAME_MAX];
    char data[PSTATUS_MAX];
    size_t rsize;
    int fd;
    char statuspath[STATUS_MAX];
    char abs_fpath[PATH_MAX];

    // verify arguments
    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "use: %s filepath\n", argv[0]);
        return EXIT_FAILURE;
    }

    // absolute file path
    if (realpath(argv[1], abs_fpath) == NULL)
        exit_err("realpath");

    // open
    proc = opendir("/proc");
    if (proc == NULL)
        exit_err("opendir:/proc");

    // walk though /proc 
    // verify file has opened by process
    // and show information
    while ((subdir = readdir(proc)) != NULL) {

        // verify proc dir
        if (!isnumberic(subdir->d_name))
            continue;

        // verify number of file has opend by process
        if (fdfproc(subdir->d_name, abs_fpath) != 1)
            continue;

        // open
        snprintf(statuspath, STATUS_MAX, "/proc/%s/status", subdir->d_name);
        fd = open(statuspath, O_RDONLY);
        if (fd == -1)
            continue;

        // read
        rsize = read(fd, data, PSTATUS_MAX);
        if (rsize < 0)
            continue;

        // get name of process
        if (pnamefstatus(data, rsize, pname) == -1)
            continue;

        // show
        printf("%s - %s\n", pname, subdir->d_name);
    }

    return EXIT_SUCCESS;
}

size_t fdmax(void)
{
    return 4096;
}

int fdfproc(char *spid, char *path)
{
    // variables
    DIR *dir;
    char fdpath[PATH_MAX];
    char fdsubpath[PATH_MAX];
    struct dirent *subfile;
    size_t rsize;
    char link[PATH_MAX];

    // open dir
    snprintf(fdpath, PATH_MAX, "/proc/%s/fd", spid);
    dir = opendir(fdpath);
    if (dir == NULL)
        return RET_FAILURE;

    // walk though fd directory
    // readlink fd and find link
    while ((subfile = readdir(dir)) != NULL) {

        // skip though file is not link
        if (subfile->d_type != DT_LNK)
            continue;

        // readlink
        snprintf(fdsubpath, PATH_MAX, "/proc/%s/fd/%s", spid, subfile->d_name);
        rsize = readlink(fdsubpath, link, PATH_MAX);
        if (rsize == -1)
           return RET_FAILURE; 
        link[rsize] = '\0';

        // verify
        if (strcmp(link, path) == 0)
            return 1;
    }
    
    // file is not opening by any processes
    return 0;
}

int pnamefstatus(char *data, size_t size, char *name)
{
    // variables
    int i;
    int namesize;

    // init
    i = 0;
    namesize = 0;

    // walk though data and find name of processing
    while (i < size) {

        // find tag
        if (strncmp(&data[i], PNAME_TAG, sizeof(PNAME_TAG) - 1) != 0) {

            // skip to end of line
            while (data[i] != '\n')
                i += 1;

            // point to new line
            i += 1;

            continue;
        }

        // skip tag
        i += sizeof(PNAME_TAG) - 1;

        // skip space
        // and point to start character of name
        while (data[i] == ' ')
            i += 1;
        i += 1;

        // calculate size of name
        while (data[i] != '\n') {
            namesize += 1;
            i += 1;
        }
        
        // point to start of name
        i -= namesize;

        // copy name to output
        memcpy(name, &data[i], namesize);
        name[namesize] = '\0';

        // success
        return EXIT_SUCCESS;
    }

    // error
    return EXIT_FAILURE;
}

int isnumberic(const char *str)
{
    while (*str != '\0') {
        if (!isdigit(*str))
            return 0;
        str += 1;
    }

    return 1;
}
