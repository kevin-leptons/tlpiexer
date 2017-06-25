/*
SYNOPSIS

    Solution for excercise 35-03.

REQUIREMENT

    Write a program that places itself under the SCHED_FIFO scheduling policy
    and then creates a child process. Both processes should execute a function
    that causes the process to consume a maximum of 3 seconds of cpu time
    (This can be done by using a loop in which the times() system call 
    is repeatedly called to determine the amount of cpu time so far consumed).
    After each quarter of a second of consumed cpu time, the function should
    print a message that displays the process id and the amount of cpu time 
    so far consumed. After each second of consumed cpu time, the function 
    should call sched_yield() to yield the cpu to the other process.
    (alternatively, the processes could raise each other's scheduling priority
    using shced_setparam()) the program's output should demonstrate that the
    two processes alternately execute for 1 second of cpu time.
    (Note carefully the advice given in modifying and retrieving policies and
    priorities about preventing a runaway realtime process from hogging the
    cpu).

SOLUTION

    - disable buffer for stdout, it make correct output when multiprocess
      write to stdout
    - parse options
    - verify arguments
    - place itself under SCHED_FIFO by sched_setscheduler()
    - drop privilege by setresuid()
    - put process execute on same of cpu by sched_setaffinity(). 
      chose last cpu to avoid concurency run with other processes and lagging
    - create child process by fork(). both parent and child process 
      fall to next statement to do same thing
    - loop to consume cpu time. detect cpu time by times().
      each 1/4 second, show information about cpu time. 
      each 1 second, call sched_yield() to transfer cpu for other processes
    - parent wait for child process has exited for easy to read output

    As output result, parent and child process use one cpu alternately each 
    second until done. Because perform in one process, real time of
    processing will more than 3 + 3 = 6 seconds.

USAGE    :

    # system may be non-responsible in a moment
    $ ./dest/bin/35-03
    pid=20817, utime=25
    pid=20817, utime=50
    pid=20817, utime=75
    pid=20817, utime=100

    pid=20818, utime=25
    pid=20818, utime=50
    pid=20818, utime=75
    pid=20818, utime=100

    pid=20817, utime=125
    pid=20817, utime=150
    pid=20817, utime=175
    pid=20817, utime=200

    pid=20818, utime=125
    pid=20818, utime=150
    pid=20818, utime=175
    pid=20818, utime=200

    pid=20817, utime=225
    pid=20817, utime=250
    pid=20817, utime=275
    pid=20817, utime=300

    pid=20818, utime=225
    pid=20818, utime=250
    pid=20818, utime=275
    pid=20818, utime=300

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <sys/times.h>
#include <sys/wait.h>

#include <tlpiexer/error.h>

#define CPU_CONSUME_TIME 3

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

int main(int argc, char *argv[])
{
    // variables
    int opt;
    struct sched_param sparam;
    struct tms time_buf;
    clock_t pre_utime;
    float clock_tick;
    pid_t ppid, pid;
    float etime;
    int ytime;
    cpu_set_t cpuset;

    // init
    pre_utime = 0;
    clock_tick = (float) sysconf(_SC_CLK_TCK);
    ppid = getpid();

    // do not use buffer for stdout
    setbuf(stdout, NULL);

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
    if (argc > 2) {
        showhelp();
        return EXIT_FAILURE;
    }

    // places itself under SCHED_FIFO
    sparam.sched_priority = 20;
    if (sched_setscheduler(0, SCHED_FIFO, &sparam) == -1)
        exit_err("sched_setscheduler");

    // drop privilege
    if (setresuid(getpid(), getpid(), getpid()) == -1)
        exit_err("setresuid");

    // put process execute on same of cpu
    // use last cpu to avoid lagging
    // be carefully in system which have only cpu, it may be stop interactive
    CPU_ZERO(&cpuset);
    CPU_SET(sysconf(_SC_NPROCESSORS_ONLN) - 1, &cpuset);
    if (sched_setaffinity(0, sizeof(cpuset), &cpuset) == -1)
        exit_err("sched_setaffinity");

    // create child process
    // both do same thing in next statements
    switch (fork()) {
        case -1: 
            exit_err("fork");
        case 0:
            // child process continue here
            // fall to next statement
            break;
        default:
            // parent process continue here
            // fall to next statement
            break;
    }

    // init
    pid = getpid();
    ytime = 0;

    // loop to consume cpu time and show information
    for (;;) {

        // detect time used by cpu
        if (times(&time_buf) == -1)
            exit_err("times");

        // calucate eslapse time from pre point
        etime = (time_buf.tms_utime - pre_utime) / clock_tick;
        
        // show information
        if (etime == 0.25) {
            printf("pid=%ld, utime=%ld\n", 
                    (long) pid, time_buf.tms_utime);

            // reset pre cpu time
            pre_utime = time_buf.tms_utime;

            // add point to yield time
            // 4 point for call sched_yield()
            ytime++;
        }

        // transfer cpu for other process
        if (ytime == 4) {
            // make output easy to read
            printf("\n");

            sched_yield();

            // reset yield time
            ytime = 0;
        }

        // verify cpu time to exit loop
        if ((time_buf.tms_utime / clock_tick) >= CPU_CONSUME_TIME)
           break; 
    }

    // parent must wait child to easy to read output
    if (pid == ppid)
        wait(NULL);

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "35-3";

    printf("\nUSAGE:\n\n");
    printf("%s\n", exename);
    printf("%s -h\n\n", exename);
}
