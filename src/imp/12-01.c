/*
SYNOPSIS

    Solution for exercise 12-01.

REQUIREMENT

    Write a program that lists the process id and command name for all
    processes being run by the user named in the program's command-line.
    Argument (You may the userIdFromName() function from listing 8-1, on 
    page 159, useful). This can be done by inspecting the 'Name:' and 'Uid:'
    lines of all of the /proc/PID/status file on the system. Walking through
    all of the /proc/PID directories on the system requires the use of
    readdir(3), which is described in section 18.8. Make sure your program
    correctly handles the possibility that a /proc/PID directory disappears
    between the time that the program determines that the directory exists and
    the time that it tries to open the corresponding /proc/PID/status file.

SOLUTION

    Same as requirements.

USAGE

    $ ./dest/bin/12-01 kevin
    1188 - systemd
    /lib/systemd/systemd

    1189 - (sd-pam)
    (sd-pam)

    1193 - gnome-keyring-d
    /usr/bin/gnome-keyring-daemon

    1196 - gnome-session-
    gnome-session

    1251 - ssh-agents
    /usr/bin/ssh-agent

    ...

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>

#define STATUS_PATH_MAX 64
#define STATUS_FILE_SIZE 4096
#define CMD_PATH_MAX 64
#define PROCESS_NAME_MAX 128
#define BUF_SIZE 1024

// spec     : get user id from username
// arg      :
//  - name: username
// ret      : user id
uid_t uidfname(const char *name);

// spec     : verify string is numberic
// arg      :
//  - str: string to verify
// ret      :
//  - other 0 on string is numberic
//  - 0 on string is numberic
int isnumber(const char *str);

// spec     : verify user owner process directory. if true, show information
//            to stdout
// arg      :
//  - procdir: process directory
//  - uid: user id
// ret      : none
void verifyproc(const char *procdir, uid_t uid);

// spec     : get user id from content of status file
// arg      :
//  - data: content of status file as string
//  - size: size of data in bytes
// ret      : user id
uid_t uid_from_sfile(char *data, size_t size);

// spec     : get process id from content of status file
// arg      :
//  - data: content of status file as string
//  - size: size of data in bytes
// ret      : process id
pid_t pid_from_sfile(char *data, size_t size);

// spec     : get name of process from content of status file
// arg      :
//  - data: content of status file as string
//  - size: size of data as bytes
//  - name: output, name of process
// ret      :
//  - 0 on success
//  - -1 on error
int name_from_sfile(char *data, size_t size, char *name);

// spec     : get process command-line from process directory
// arg      :
//  - procdir: process directory in /proc/pid
// ret      : none
void pcmd_from_proc(const char *procdir);

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

int main(int argc, char **argv)
{
    // variables
    DIR *proc;
    uid_t uid;
    struct dirent *subdir;

    // show help
    if (argc == 2 && strcmp(argv[1], "-h") == 0) {
        showhelp();
        return EXIT_SUCCESS;
    }

    // verify argument
    if (argc != 2) {
        showhelp();
        return EXIT_FAILURE;
    }

    // get user id from name
    uid = uidfname(argv[1]); 
    if (uid == -1) {
        fprintf(stdout, "error: uidfname");
        return EXIT_FAILURE;
    }

    // open
    proc = opendir("/proc");
    if (proc == NULL) {
        perror("opendir");
        return EXIT_FAILURE;
    }

    // walk though /proc/<pid>/status
    // and filter process have user id match with target
    errno = 0;
    while ((subdir = readdir(proc)) != NULL) {
        if (errno != 0) {
            perror("readdir");
            return EXIT_FAILURE;
        }
                
        verifyproc(subdir->d_name, uid);

        errno = 0;
    }
    
    // success
    return EXIT_SUCCESS;
}

uid_t uidfname(const char *name)
{
    // variables
    struct passwd *pwd;
    uid_t uid;
    char *endptr;

    if (name == NULL || *name == '\0')
        return -1;

    uid = strtol(name, &endptr, 10);;
    if (*endptr == '\0')
        return uid;

    pwd = getpwnam(name);
    if (pwd == NULL)
        return -1;

    return pwd->pw_uid;
}

int isnumber(const char *str)
{
    while (*str != '\0') {
        if (!isdigit(*str)) 
            return 0;
        str += 1;
    }

    return 1;
}

void verifyproc(const char *dirname, uid_t uid)
{
    // variables
    uid_t puid;
    pid_t pid;
    int fd;
    char status_path[STATUS_PATH_MAX];
    char data[STATUS_FILE_SIZE];
    size_t datasize;
    char pname[PROCESS_NAME_MAX];

    // verify processing directory
    if (!isnumber(dirname))
        return;

    // open status file
    snprintf(status_path, STATUS_PATH_MAX, "/proc/%s/status", dirname);
    fd = open(status_path, O_RDONLY); 
    if (fd == -1)
        return;

    // read status file data
    datasize = read(fd, data, STATUS_FILE_SIZE);
    if (datasize == -1)
        return;
    if (close(fd) == -1)
        return;

    // get user id which own process
    puid = uid_from_sfile(data, datasize);
    if (puid == -1)
        return;

    // verify uid
    if (puid != uid)
        return;

    // get process id
    pid = pid_from_sfile(data, datasize);

    // get name of process
    if (name_from_sfile(data, datasize, pname) == -1)
            return;

    // show
    printf("%i - %s\n", pid, pname);
    pcmd_from_proc(dirname);
    printf("\n");
}

uid_t uid_from_sfile(char *data, size_t size)
{
    // variables
    int i;

    // init
    i = 0;

    // walk though data and find user id
    while (i < size) {

        // verify uid tag
        if (strncmp(&data[i], "Uid:", 4) != 0) {
            // skip to line break
            // and point cursor to new line
            while (data[i] != '\n') 
                i += 1;
            i += 1;

            continue;
        }

        // skip though tag
        i += 4;

        // skip though space
        while (data[i] == ' ')
            i += 1;
        i += 1;
        
        // convert first value
        // that is uid
        return strtol(&data[i], NULL, 10);
    } 

    return EXIT_FAILURE;
}

pid_t pid_from_sfile(char *data, size_t size)
{
    // variables
    int i;

    // init
    i = 0;

    // walk though data and find pid
    while (i < size) {

        // verify pid tag
        if (strncmp(&data[i], "Pid:", 4) != 0) {
            // skip to line break
            // and pint cursor to new line
            while (data[i] != '\n')
                i += 1;
            i += 1;

            continue;
        }

        // skip though tag
        i += 4;

        // skip though space
        while (data[i] == ' ')
            i += 1;
        i += 1;

        // convert string value to number
        return strtol(&data[i], NULL, 10);
    }

    return EXIT_FAILURE;
}


void pcmd_from_proc(const char *procdir)
{
    // variables
    int fd;
    char cmd_path[CMD_PATH_MAX];
    int rsize;
    char buf[BUF_SIZE];

    // prepare path
    snprintf(cmd_path, CMD_PATH_MAX, "/proc/%s/cmdline", procdir);

    // open
    fd = open(cmd_path, O_RDONLY); 
    if (fd == -1) {
        perror("open");
        return;
    }

    // read and print
    while ((rsize = read(fd, buf, BUF_SIZE)) > 0) {
        /*buf[rsize + 1] = NULL;*/
        printf("%s", buf);
    } 
    printf("\n");
    if (rsize == -1) {
        perror("read");
        return;
    }

    // close
    close(fd);
}

int name_from_sfile(char *data, size_t size, char *name)
{
    // variables
    int i;
    int namesize;

    // init
    i = 0;
    namesize = 0;

    // walk though data and find pid
    while (i < size) {

        // verify pid tag
        if (strncmp(&data[i], "Name:", 5) != 0) {
            // skip to line break
            // and pint cursor to new line
            while (data[i] != '\n')
                i += 1;
            i += 1;

            continue;
        }

        // skip though tag
        i += 5;

        // skip though space
        while (data[i] == ' ')
            i += 1;
        i += 1;

        // find size of name
        while (data[i] != '\n') {
            i += 1;
            namesize += 1;
        }
        
        // copy name to output
        memcpy(name, &data[i - namesize], namesize);
        name[namesize + 1] = '\0';
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}

void showhelp()
{
    // variables
    char exename[] = "12-1";

    printf("\nUSAGE:\n\n");
    printf("\t%s username\n", exename);
    printf("\t%s -h\n\n", exename);

    printf("ARGUMENTS:\n\n");
    printf("\tusername: specify username of processes will be show\n\n");
}
