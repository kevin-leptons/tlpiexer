/*
SYNOPSIS

    Solution for exercise 36-03.

REQUIREMENT

    Write programs to determine what happens if a process's consumption of
    various resources already exceeds the soft limit specified in a call to
    setrlimit().

SOLUTION

    - verify arguments
    - create child_1 process. in that process, set limit for address space
      then try invoke more than that. it must fails
    - create child_2 process. in that process, set limit for cpu time then
      try consume cpu time more than that. kernel will send SIGXCPU, then
      send SIGKILL to child_2 process. it must be killed by kernel

USAGE

    $ ./dest/bin/36-03
    pid=4954: parent

    pid=4955: limit RLIMIT_AS
    Error: do_limit_as:Cannot allocate memory
    pid=4955: exited, status=1

    pid=4956: limit RLIMIT_CPU
    pid=4956: catch 24. CPU time limit exceeded
    pid=4956: catch 24. CPU time limit exceeded
    pid=4956: catch 24. CPU time limit exceeded
    pid=4956: killed by signal 9. Killed
    
AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <sys/wait.h>

#include <tlpiexer/error.h>

#define RET_SUCCESS 0
#define RET_FAILURE -1

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : create child process, in child process try set limit for resource
//            and use more than that. parent must call wait(2) to retrieve
//            return status of child process
// arg      :
//  - resource: type of resource will be test
// ret      :
//  - 0 on success
//  - -1 on fails
int do_overload(int resource);

// spec     : set of function to try use more than resource is limited
// arg      : none
// ret      :
//  - 0 on success
//  - -1 on error
int do_limit_as();
int do_limit_cpu();

// spec     : show process exit status
// arg      : 
// - pid: identity of exited process
//  - status: exit status of process from wait(2)
// ret      : none
void dump_status(pid_t pid, int status);

// spec     : callback function will be call when signal catched
// art      :
//  - sig: signal number
// ret      : none
void sighandler(int sig);

int main(int argc, char *argv[])
{
    // variables
    int opt;
    int resources[] = { RLIMIT_AS, RLIMIT_CPU };
    int i;
    int child_exit;
    pid_t cpid;
    int limit_size;

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

    // verify no arguments has provided
    if (argc > 1) {
        showhelp();
        return EXIT_FAILURE;
    }

    // show information
    printf("pid=%i: parent\n\n", getpid());

    // init
    limit_size = sizeof(resources)/sizeof(int);

    // disable buffer of stdout
    // it make ouput correctly
    setbuf(stdout, NULL);

    // create child process, that try to raise some operation to use more
    // than resource is limited
    for (i = 0; i < limit_size; i++) {
        if (do_overload(resources[i]) == -1)
            exit_err("do_overload");

        // wait for child process exited
        // then show exit status of them
        if ((cpid = wait(&child_exit)) == -1)
            exit_err("wait");

        dump_status(cpid, child_exit);
        printf("\n");
    }

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "36-1";

    printf("\nUSAGE:\n\n");
    printf("\t%s\n", exename);
    printf("\t%s -h\n\n", exename);
}

int do_overload(int resource)
{
    // variables
    
    // create child process
    switch (fork()) {
        case -1:
            // fails
            return RET_FAILURE;
        case 0:
            // child process continue here
            
            // use more resource more than limited here
            switch (resource) {
                case RLIMIT_AS: 
                    if (do_limit_as() == -1)
                        exit_err("do_limit_as");
                    break;
                case RLIMIT_CPU: 
                    if (do_limit_cpu() == -1)
                        exit_err("do_limit_as");
                    break;
                default: _exit(EXIT_FAILURE); 
            }

            // success
            _exit(EXIT_SUCCESS);
        default:
            // parent process continue here
            
            // success
            return RET_SUCCESS;
    }
}

void dump_status(pid_t pid, int status)
{
    if (WIFEXITED(status))
        printf("pid=%i: exited, status=%d\n", pid, WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        printf("pid=%i: killed by signal %d. %s\n", pid,
                WTERMSIG(status), strsignal(WTERMSIG(status)));
    else if (WIFSTOPPED(status))
        printf("pid=%i: stopped by signal %d. %s\n", pid,
                WSTOPSIG(status), strsignal(WSTOPSIG(status)));
    else if (WIFCONTINUED(status))
        printf("pid=%i: continue\n", pid);
    else
        printf("pid=%i: unknow exit, status=%x\n", pid,
                (unsigned int)status);
}

int do_limit_as()
{
    // variables
    struct rlimit limit;

    // show information
    printf("pid=%i: limit RLIMIT_AS\n", getpid());

    // limit memory usage 
    limit.rlim_cur = 2*1024*1024;
    limit.rlim_max = limit.rlim_cur;
    if (setrlimit(RLIMIT_AS, &limit) == -1)
        return RET_FAILURE;

    // try use more than memory has limited
    if (malloc(4*1024*1024) == NULL)
        return RET_FAILURE;

    // success
    return RET_SUCCESS;
}

int do_limit_cpu()
{
    // variables
    struct rlimit limit;
    struct tms t;
    float clock_tick;
    struct sigaction sigact;

    // show information
    printf("pid=%i: limit RLIMIT_CPU\n", getpid());

    // init
    clock_tick = sysconf(_SC_CLK_TCK);

    // signal handler for SIGXCPU
    sigact.sa_handler = sighandler;
    sigact.sa_flags = 0;
    sigemptyset(&sigact.sa_mask);
    if (sigaction(SIGXCPU, &sigact, NULL) == -1)
        return RET_FAILURE;

    // limit cpu time unit by second
    limit.rlim_cur = 3;
    limit.rlim_max = 6;
    if (setrlimit(RLIMIT_CPU, &limit) == -1)
        return RET_FAILURE;

    // try use more than cpu time has limited
    // during consume cpu time, kernel will send SIGXCPU and SIGKILL 
    // to process
    t.tms_utime = 0;
    for (; t.tms_utime / clock_tick < 8;)
        if (times(&t) == -1)
            return RET_FAILURE;
    
    // success
    return RET_SUCCESS;
}

void sighandler(int sig)
{
    // unsafe
    printf("pid=%i: catch %i. %s\n", 
            getpid(), sig, strsignal(sig));
}
