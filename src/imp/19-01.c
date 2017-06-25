/*
SYNOPSIS

    Solution for exercise 19-01.

REQUIREMENT

    Write a program that logs all file creations, deletetions, and renames
    under the directory named in its command-line argument. The program
    should monitor events in all of the subdirectories under the specified
    directory. To obtain a list of all of these subdirectories, you will need
    to make use of nftw() (section 18.9). When a new subdirectory is added
    under the tree or directory is deleted, the set of monitored
    sucdirectories should be updated accordingly.

SOLUTION :

    - prepare file event by call inotify_init()
    - call nftw() to walkthough directory and call inotify_add_watch() to
      watch file events
    - create infinite loop, in loop call read() event file descriptor to get 
      information about file events
    - show information of file events
    - if directory has been deleted/created, call inotify_rm_watch() or
      inotify_add_watch() to remove/add watch event

USAGE

    # watch directory in background
    $ ./dest/bin/19-01 tmp/ &
    [1] 18354

    $ touch tmp/new-file.txt
    name = new-file.txt
    wd = 1; mask = IN_CREATE; cookies = 0

    $ mv tmp/new-file.txt tmp/old-file.txt
    name = new-file.txt
    wd = 1; mask = IN_MOVED_FROM; cookies = 4086
    name = old-file.txt
    wd = 1; mask = IN_MOVED_TO; cookies = 4086
    
    $ rm tmp/old-file.txt
    name = old-file.txt
    wd = 1; mask = IN_DELETE; cookies = 0

    $ kill %1

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#define _XOPEN_SOURCE 500

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <ftw.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <sys/inotify.h>

#include <tlpiexer/error.h>

#define RET_SUCCESS 0
#define RET_FAILURE -1
#define BUF_LEN (10*(sizeof(struct inotify_event) + NAME_MAX + 1))
#define MAX_WD 82192

// spec     : this block is global variable. it use for inotify event
int ifd;
int inotify_flags = IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MOVE | 
    IN_MOVE_SELF;
char *dpath[MAX_WD];

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : event callback function. it is will call for each inotify event 
//            from read() function
// arg      : 
//  - e: array of event
// rete     : none
static void 
eventcb(struct inotify_event *e);

// spec     : entry callback function. it is will call for each entry of
//            directory
// arg      :
//  - fpath: file path, it is relative path with current directory
//  - status: status of file
//  - type: type flag
//  - ftw: contains level of file from walk directory
// ret      :
//  - 0 on success, nftw will continue
//  - other on error, nftw will stop
int 
entrycb(const char *path, const struct stat *status, int typt,
        struct FTW *ftw);

int main(int argc, char **argv)
{
    // variables
    int opt;
    char *dirpath;
    ssize_t nread;
    char buf[BUF_LEN];
    struct inotify_event *e;
    char *seeker;
    int i;

    // init
    memset(dpath, 0, sizeof(dpath));

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

    // verify path to directory will be watch events
    if (optind >= argc || argc == 1)
        dirpath = (char*)&".";
    else
        dirpath = argv[optind];

    // create inotify
    ifd = inotify_init();
    if (ifd== -1)
        exit_err("inotify_init");

    // walk though file and add watch directory
    if (nftw(dirpath, entrycb, 8, FTW_PHYS) == -1)
        exit_err("nftw");

    // infinite loop to receive events from file hash changed
    for(;;) {
        // wait when event has occur
        nread = read(ifd, buf, BUF_LEN);
        if (nread < 0)
            exit_err("read");

        // process all of events returned by inotify
        for(seeker = buf; seeker < buf + nread;) {
            e = (struct inotify_event*)seeker;
            eventcb(e);

            seeker += sizeof(struct inotify_event) + e->len;
        }
    }

    // release
    for (i = 0; i < MAX_WD; i++)
        if (dpath[i] != NULL)
            free(dpath[i]);

    // success
    return EXIT_SUCCESS;
}

static void 
eventcb(struct inotify_event *e)
{
    // variables
    char *mask;
    char ndpath[PATH_MAX];
    int nwd;

    // convert mask to string
    if ((e->mask & IN_CREATE) == IN_CREATE)
        mask = (char*)&"IN_CREATE";
    else if ((e->mask & IN_DELETE) == IN_DELETE)
        mask = (char*)&"IN_DELETE";
    else if ((e->mask & IN_DELETE_SELF) == IN_DELETE_SELF)
        mask = (char*)&"IN_DELETE_SELF";
    else if ((e->mask & IN_MOVED_TO) == IN_MOVED_TO)
        mask = (char*)&"IN_MOVED_TO";
    else if ((e->mask & IN_MOVED_FROM) == IN_MOVED_FROM)
        mask = (char*)&"IN_MOVED_FROM";
    else if ((e->mask & IN_MOVE_SELF) == IN_MOVE_SELF)
        mask = (char*)&"IN_MOVE_SELF";
    else
        mask = (char*)&"EVENT IS UNWATCHED";

    // add directory to watch
    if (((e->mask & IN_CREATE) == IN_CREATE) && 
            ((e->mask & IN_ISDIR) == IN_ISDIR)) {

        // prepare new path
        strcpy(ndpath, dpath[e->wd]);
        strcat(ndpath, "/");
        strcat(ndpath, e->name);

        // watch
        if ((nwd = inotify_add_watch(ifd, ndpath, inotify_flags) == -1))
            exit_err("inotify_add_watch");

        // index
        dpath[nwd] = malloc(strlen(ndpath));
        strcpy(dpath[nwd], ndpath);
    }

    // remove directory to watch
    if (((e->mask & IN_DELETE) == IN_DELETE) && 
        ((e->mask & IN_ISDIR) == IN_ISDIR)) {

        // remove watch
        if (inotify_rm_watch(ifd, e->wd) == -1)
            exit_err("inotify_rm_watch");

        // remove index
        free(dpath[e->wd]);
        dpath[e->wd] = NULL;
    }

    // show
    printf("name = %s\nwd = %d; mask = %s; cookies = %d\n", 
            e->name, e->wd, mask, e->cookie);
}

int
entrycb(const char *path, const struct stat *status, int type, struct FTW *ftw)
{
    // variables
    int wd;

    // verify file is directory
    if (!S_ISDIR(status->st_mode))
        return RET_SUCCESS;

    // add watch for directory and sub file
    if ((wd = inotify_add_watch(ifd, path, inotify_flags)) == -1)
        return RET_FAILURE;

    // index directory path
    dpath[wd] = (char*)malloc(PATH_MAX);
    realpath(path, dpath[wd]);

    // success
    return RET_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "19-1";

    printf("\nUSAGE:\n\n");
    printf("%s dir\n", exename);
    printf("%s -h\n\n", exename);

    printf("ARGUMENTS:\n\n");
    printf("dir: path to directory to watch events of files\n\n");
}
