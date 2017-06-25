/*
SYNOPSIS

    Solution for exercise 35-04.

REQUIREMENT

    If two process use a pipe to exchange a large amount of data on a
    multiprocessor system, the communication should be faster if the processes
    run on the same cpu than if they run on different cpus. The reason is
    that when two processes run on the same cpu, the pipe data will be more 
    quickly accessed because it can remain in that cpu's cache. By constrant,
    when the processes run on separate cpus, the benefits of the cpu cache
    are lost. If you have access to a multiprocessor system, write a program
    that uses sched_setaffinity() to demontrate this effect, by forcing
    the processes either onto the same cpus or onto different cpus
    (Chapter 44 describes the use of pipes).

SOLUTION

    - detect number of cpu
    - verify number of cpu for test
    - put current process run on only cpu by sched_setaffinity()
    - disable stdout buffer, so output will easy to read
    - first case: create pipe by pipe() then create child process by fork(). 
      do no put it run on different cpu than parent. parent process must 
      transfer SEND_TIMES times though pipe to child process, 
      each time transfer BUF_SIZE bytes. during that, measures and show time 
      usage. parent must wait for child exited
    - second case: create pipe by pipe(). then create child process by fork(). 
      put it run on different cpu than parent. parent process must transfer 
      SEND_TIMES times though pipe to child process, each time transfer 
      BUF_SIZE bytes. during that, measures and show time usage.
      parent must wait for child exited

    Output must point that two process run on same processor more quickly than
    two process run on different processor.

    However, on cpus shared cache, result may be not more different.

USAGE

    $ ./dest/bin/35-04
    wait a moment, testing need long time to show different
    parent: pid=28240, cpuid=3, sending data
    child: pid=28241, cpuid=3, receiving data
    pipe on diff process: 16777216 times on 16 secs, 254835 usec

    parent: pid=28240, cpuid=3, sending data
    child: pid=28280, cpuid=2, receiving data
    pipe on diff process: 16777216 times on 17 secs, 375785 usec

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <sys/time.h>
#include <sys/wait.h>

#include <tlpiexer/error.h>

#define RET_SUCCESS 0
#define RET_FAILURE -1
#define BUF_SIZE 4096
#define SEND_TIMES 16*1024*1024

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : perform transfer data between processes
// arg      : 
//  - p_cpuid: parent cpu id
//  - c_cpuid: child cpu id
// ret      :
//  - 0 on success
//  - -1 on fail
int do_pipe(int p_cpuid, int c_cpuid);

int main(int argc, char *argv[])
{
    // variables
    int opt;
    int ncpu;
    cpu_set_t cpuset;

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

    // init
    ncpu = sysconf(_SC_NPROCESSORS_ONLN);

    // verify number of cpu
    if (ncpu < 4) {
        fprintf(stderr, "number of cpu = %i, can not or not safe to run\n", 
                ncpu);
        return EXIT_FAILURE;
    }

    // show information
    printf("wait a moment, testing need long time to show different\n");

    // put process run on fixed cpu
    // chose last cpu
    CPU_ZERO(&cpuset);
    CPU_SET(ncpu - 1, &cpuset);
    if (sched_setaffinity(0, sizeof(cpuset), &cpuset) == -1)
        exit_err("sched_setaffinity");

    // disable buffer of stdout
    setbuf(stdout, NULL);

    // first case, perform transfer data though pipe on same processor
    if (do_pipe(ncpu - 1, ncpu - 1) == -1)
        exit_err("pipe_same_cpu");

    // second case, perform transfer data though pipe on different processor
    // parent use p_cpuid processor, so child must use other cpu
    // on multi-threading cpu, one physical cpu can have two logical cpu
    // to sure parent and child run on different physical cpu,
    // pic p_cpuid - 2 processor
    if (do_pipe(ncpu - 1, ncpu - 2) == -1)
        exit_err("pipe_diff_cpu");

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "35-4";

    printf("\nUSAGE:\n\n");
    printf("%s\n", exename);
    printf("%s -h\n\n", exename);
}

int do_pipe(int p_cpuid, int c_cpuid)
{
    // variables
    struct timeval start, end;
    int i;
    int pipefd[2];
    cpu_set_t cpuset;
    char buf[BUF_SIZE];
    ssize_t sec, nsec;

    // create pipe line
    if (pipe(pipefd) == -1)
        exit_err("pipe");

    // creat child process
    switch (fork()) {
        case -1:
            // fails
            exit_err("fork");
        case 0:
            // child process continue here
            
            // close write pipe fd to avoid error
            if (close(pipefd[1]) == - 1)
                exit_err("close");

            // put child process run on specific cpu
            CPU_ZERO(&cpuset);
            CPU_SET(c_cpuid, &cpuset);
            if (sched_setaffinity(0, sizeof(cpuset), &cpuset) == -1)
                exit_err("sched_setaffinity");

            // show information
            printf("child: pid=%i, cpuid=%i, receiving data\n", 
                    getpid(), c_cpuid);

            // detect start time
            if (gettimeofday(&start, NULL) == -1)
                exit_err("gettimeofday");

            // read data from pipe
            for (i = 0; i < SEND_TIMES; i++) {
                if (read(pipefd[0], buf, BUF_SIZE) == -1)
                    exit_err("read");
            }

            // detect end time and show result
            if (gettimeofday(&end, NULL) == -1)
                exit_err("gettimeofday");
            sec = end.tv_sec - start.tv_sec;
            nsec = end.tv_usec - start.tv_usec;
            if (nsec < 0) {
                sec -= 1;
                nsec += 1000000;
            }
            printf("pipe on diff process: %i times on %ld secs, %ld usec\n",
                    SEND_TIMES, sec, nsec);

            // success
            _exit(EXIT_SUCCESS);
        default:
            // parent process continue here
            
            // close read pipe fd to avoid error
            if (close(pipefd[0]) == -1)
                exit_err("close");

            // show information
            printf("parent: pid=%i, cpuid=%i, sending data\n", 
                    getpid(), p_cpuid);

            // send data to child though pipe
            // use new allocated block
            for (i = 0; i < SEND_TIMES; i++) {
                if (write(pipefd[1], buf, BUF_SIZE) == -1)
                    exit_err("write");
            }

            // wait for child has received all of data and exited
            // make output easy to read
            wait(NULL);
            printf("\n");

            // success
            return RET_SUCCESS;
    }
}
