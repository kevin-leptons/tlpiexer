/*
SYNOPSIS

    Solution for exercise 12-02.

REQUIREMENT

    Write a program that draws a tree showing the hierarchical parent-child
    relationship of all processes on the system, going all the way back to
    init. For each process, the program should display the process id and
    the command being executed. The output of the program shouldimilar to
    that produced by pstree(1), although it does need not to be as
    sophisticated. The parent of each process on the system can be found by
    inspecting the "PPid:" line of all of the /proc/pid/status files on the
    system. Be careful to handle the possibility that a process's parent
    (and thus its /pro/pid/directory) disappears during the scan of all
    /proc/pid directories.

SOLUTION

    Same as hints of requirements

USAGE

    $ ./dest/bin/12-02
    init - 0
    systemd - 1
        systemd-journal - 185
        systemd-udevd - 192
        rpcbind - 527
        rpc.statd - 537
        rpc.idmapd - 563
        accounts-daemon - 566
        mongod - 567
        ...
    ...

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <ctype.h>

#define RET_SUCCESS 0
#define RET_FAILURE -1
#define PROCESS_NAME_MAX 128
#define STATUSPATH_MAX 64
#define STATUSSIZE_MAX 4096
#define PNAME_TAG "Name:"
#define PID_TAG "Pid:"
#define PPID_TAG "PPid:"

// spec     : contains information of process
struct pinfo_t
{
    pid_t pid;
    pid_t ppid;
    char name[PROCESS_NAME_MAX];
};

// spec     : collect all of processes information into array
// arg      :
//  - pinfo: array of processes information
//  - size: size of pinfo array
// ret      :
//  - non-negetive number of processes read into pinfo
//  - -1 on error
int parseproc(struct pinfo_t *pinfo, size_t size);

// spec     : get max number of processes can run on system
// arg      : none
// ret      : number of processes
size_t maxpid(void);

// spec     : verify string is numberic
// arg      :
//  - str: stirng to verify
// ret      :
//  - non-zero on string is numberic
//  - 0 on string is not numberic
int isnumberic(const char *str);

// spec     : get process id from content of status file
// arg      :
//  - data: content of status file
//  - size: size of data
// ret      : process id
pid_t pidfstatus(char *data, size_t size);

// spec     : get id of parent of process from content of status file
// arg      :
//  - data: content of status file
//  - size: size of data
// ret      : id of parent of process
pid_t ppidfstatus(char *data, size_t size);

// spec     : get name of process from content of status file
// arg      :
//  - data: content of status file
//  - size: size of data
//  - name: output, contain name of process
// ret      :
//  - 0 on success
//  - -1 on error
int pnamefstatus(char *data, size_t size, char *name);

// spec     : show tree of process to stdout. this function will call
//            self-recursive to show tree
// arg      :
//  - level: level of tree, base on zero
//  - parent: any node in tree
//  - list: list of all of processes
//  - size: size of list
void ptree(int level, struct pinfo_t *parent, struct pinfo_t *list,
        size_t size);

int main(int argc, char **argv)
{

    // variables
    struct pinfo_t *pinfo;
    size_t numproc;
    size_t pmax;

    // init
    pmax = maxpid();
    pinfo = malloc(pmax*sizeof(struct pinfo_t));
    if (pinfo == NULL) {
        perror("malloc");
        return EXIT_FAILURE;
    }


    // parse all of process
    numproc = parseproc(pinfo, pmax);
    if (numproc == -1) {
        perror("parseproc");
        return EXIT_FAILURE;
    }

    // insert init processing
    numproc += 1;
    strcpy(pinfo[numproc].name, "init");
    pinfo[numproc].pid = 0;
    pinfo[numproc].ppid = 0;

    // dump information in tree
    ptree(0, &pinfo[numproc], pinfo, numproc);

    // success
    return EXIT_SUCCESS;
}

int parseproc(struct pinfo_t *pinfo, size_t size)
{
    // variable
    DIR *proc;
    struct dirent *subdir;
    int fd;
    char statuspath[STATUSPATH_MAX];
    size_t rsize;
    char buf[STATUSSIZE_MAX];
    int i;

    // init
    i = 0;

    // open dir
    proc = opendir("/proc");
    if (proc == NULL)
        return RET_FAILURE;

    // walk though directory in /proc
    // pick process directory with numberic
    // open /proc/<pid>/status and parse
    while ((subdir = readdir(proc)) != NULL) {

        // verify number of slot
        if (i > size)
            return RET_FAILURE;

        // verify processing directory
        if (!isnumberic(subdir->d_name))
            continue;

        // open
        snprintf(statuspath, STATUSPATH_MAX, "/proc/%s/status", subdir->d_name);
        fd = open(statuspath, O_RDONLY);
        if (fd == -1)
            continue;

        // read
        rsize = read(fd, buf, STATUSSIZE_MAX);
        if (rsize < 0)
            return RET_FAILURE;
        if (close(fd) == -1)
            return RET_FAILURE;

        // get pid
        pinfo[i].pid = pidfstatus(buf, rsize);
        if (pinfo[i].pid == -1)
            return RET_FAILURE;

        // get ppid
        pinfo[i].ppid = ppidfstatus(buf, rsize);
        if (pinfo[i].ppid == -1)
            return RET_FAILURE;

        // get name
        if (pnamefstatus(buf, size, (char*)&pinfo[i].name))
            return RET_FAILURE;

        // point to next slot
        i += 1;
    }

    // success
    return i;
}

size_t maxpid()
{
    return 4096;
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

pid_t pidfstatus(char *data, size_t size)
{
    // variables
    int i;

    // init
    i = 0;

    // walk though data and find name of processing
    while (i < size) {

        // find tag
        if (strncmp(&data[i], PID_TAG, sizeof(PID_TAG) - 1) != 0) {

            // skip to end of line
            while (data[i] != '\n')
                i += 1;

            // point to new line
            i += 1;

            continue;
        }

        // skip tag
        i += sizeof(PID_TAG) - 1;

        // skip space
        // and point to start pid
        while (data[i] == ' ')
            i += 1;
        i += 1;

        // convert string to number
        return strtol(&data[i], NULL, 10);
    }

    // error
    return EXIT_FAILURE;
}

pid_t ppidfstatus(char *data, size_t size)
{
    // variables
    int i;

    // init
    i = 0;

    // walk though data and find name of processing
    while (i < size) {

        // find tag
        if (strncmp(&data[i], PPID_TAG, sizeof(PPID_TAG) - 1) != 0) {

            // skip to end of line
            while (data[i] != '\n')
                i += 1;

            // point to new line
            i += 1;

            continue;
        }

        // skip tag
        i += sizeof(PPID_TAG) - 1;

        // skip space
        // and point to start pid
        while (data[i] == ' ')
            i += 1;
        i += 1;

        // convert string to number
        return strtol(&data[i], NULL, 10);
    }

    // error
    return EXIT_FAILURE;
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
        strncpy(name, &data[i], namesize);
        name[namesize + 1] = '\0';

        // success
        return EXIT_SUCCESS;
    }

    // error
    return EXIT_FAILURE;
}


void ptree(int level, struct pinfo_t *parent, struct pinfo_t *list, size_t size)
{
    // variables
    int i;

    // display parent
    for (i = 0; i < level; i++)
        printf("    ");
    printf("%s - %i\n", parent->name, parent->pid);

    // walk though list and display tree of all child process
    for (i = 0; i < size; i++) {

        // skip though itself parent and no-child
        if (list[i].pid == parent->pid || list[i].ppid != parent->pid)
            continue;

        // display tree of child
        ptree(level + 1, &list[i], list, size);
    }
}
