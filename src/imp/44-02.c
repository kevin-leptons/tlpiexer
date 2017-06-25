/*
SYNOPSIS

    Solution for exercise 44-02.

REQUIREMENT

    Implement popen() and pclose(). Although these function are simplified by
    not requiring the signal handling employed in the implementation of 
    system() (section 27.7), you will need to be careful to correctly bind
    the pipe ends to file streams in each process, and to ensure that all
    unused descriptors referring to the pipe ends are closed. Since children
    created by multiple calls to popen() may be running at one time, you will
    need to maintain a data structure that associates the file stream pointers 
    allocated by popen() with the corresponding child process IDs. (If using an
    array for this purpose, the value returned by the fileno() function,
    which obtains the file descriptor corresponding to a file stream, can
    be used to index the array) Obtaining the correct process ID from this
    structure will allow pclose() to select the child upon which to wait. This
    structure will also assist with the SUSv3 requirement that any still-open
    file streams created by earlier calls to popen() must be closed in the
    new child process.

SOLUTION

    IMPLEMENT popen()

        - Create a two of file descriptors by pipe()
        - Create child process by fork()

    In parent process:

        - Set maintain data of file descriptors and process identity
        - Close descriptor depend on popen() which called with "type"
        - Create file stream from file descriptor by fdopen(3)
        - Return file stream 

    In child process:

        - Close any file descriptors which are not using by required
        - Duplicate "stdin" or "stdout" by file descriptors from pipe(), depend
          on "type"
        - Replace program run in process by execl()

    IMPLEMENT pclose()

        - Convert file stream to file descriptor by fileno(3)
        - Detect process identity which is called pclose() by file descriptor
        - Close process by kill()
        - Wait for process terminate by waitpid() and save exit status
        - Close file descriptor 
        - Return exit status of child process

USAGE

    $ ./dest/bin/44-02 whoami
    kevin

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <wait.h>
#include <signal.h>

#include <tlpiexer/error.h>

#define BUF_SIZE 1024

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : see popen(3)
FILE *popen(const char *command, const char *type);

// spec     : see pclose(3)
int pclose(FILE *stream);

int main(int argc, char *argv[])
{
    char buf[BUF_SIZE];
    FILE *rfile;
    ssize_t rsize;

    // show help menu
    if (argc == 2 && strcmp(argv[1], "-h") == 0) {
        showhelp();
        return EXIT_SUCCESS;
    }

    // verify no argumetns has provided
    if (argc != 2) {
        showhelp();
        return EXIT_FAILURE;
    }

    // perform popen() and pclose()
    rfile = popen(argv[1], "r");
    if (rfile == NULL)
        exit_err("popen");
    while ((rsize = fread(buf, BUF_SIZE, BUF_SIZE, rfile)) > 0)
        printf("%s", buf);
    if (rsize < 0)
        exit_err("read");
    if (pclose(rfile) < 0)
        exit_err("pclose");

    // successfully
    return EXIT_SUCCESS;
}

// Use as maintain data for popen(3) and pclose(3)
#define MAX_FD 65535
pid_t POPEN_INDEXES[MAX_FD] = { -1 };

FILE *popen(const char *command, const char *type)
{
    int fds[2];
    int fd, ufd;
    pid_t cpid;

    // verify type of open
    if (strcmp(type, "r") != 0 && strcmp(type, "w") != 0) {
        errno = EINVAL;
        return NULL;
    }

    // create pipe
    if (pipe(fds) == -1)
        return NULL;

    // create child process to excute command has provided
    switch ((cpid = fork())) {
        case -1:
            return NULL;

        case 0:
            // child process continue here
            
            // duplicate standard file stream
            if (strcmp(type, "r") == 0) {
                close(fds[0]);
                dup2(STDOUT_FILENO, fds[1]);
            } else {
                close(fds[1]);
                dup2(STDIN_FILENO, fds[0]);
            }
            
            // replace current program with by shell
            // to excute command
            execl("/bin/sh", "sh", "-c", command, (char*) 0);
            
            // if error process will reach here
            exit(EXIT_FAILURE);
            
        default:
            // parent process continue here
            
            // detect file descriptor will be use
            if (strcmp(type, "r") == 0) {
                fd = fds[0];
                ufd = fds[1];
            } else {
                fd = fds[1];
                ufd = fds[0];
            }
            
            // close unneccessary file descriptor
            close(ufd);

            // save maintain data
            POPEN_INDEXES[fd] = cpid;

            // create file stream from file descriptor
            // and return
            return fdopen(fd, type);
    }
    
}

int pclose(FILE *stream)
{
    int fd;
    pid_t cpid;
    int status;

    // get file descriptor from file stream
    fd = fileno(stream); 

    // close file descriptor
    close(fd);

    // force write cache data in user-space to kernel-space
    fflush(stream);

    // detect child process identity
    cpid = POPEN_INDEXES[fd];

    // wait for child process exited
    if (waitpid(cpid, &status, WNOHANG) == -1) {
        return -1;
    }

    // successfully
    return cpid; 
}

void showhelp()
{
    // variables
    char exename[] = "44-2";

    printf("\nUSAGE:\n\n");
    printf("\t%s\n", exename);
    printf("\t%s -h\n\n", exename);
}
