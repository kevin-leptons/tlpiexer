/*
SYNOPSIS

    Solution for exercise 61-01.

REQUIREMENT

    Suppose that the program in Listing 61-2 (is_echo_cl.c) was modified so
    that instead of using fork() to create two processes that operate
    concurently, it instead used just one process that first copies its
    standard input to the socket and then reads the server's response. What
    problem might occur when running this client?
    (Look at Figure 58-8, on page 1190).

SOLUTION

    PREDICT

        Look at the model below:

             server                                                client 

            |      |<---( s_rbuf )<---|---------|----( c_wbuf )<---|     |
            | s_fd |                  | network |                  | cfd |
            |      |--->( s_wbuf )----|---------|--->( c_rbuf )--->|     |
                                                                      ^
                                                                      |
                                                                 | c_stdin |

        Problem occur if:

            c_stdin > c_wbuf + s_rbuf + s_wbuf + c_rbuf

        Problem occur by follow:

            Because client doesn't read data from cfd, c_rbuf will be full.
           
            When c_rbuf is full, client notify server to pause transmiting.
            Then s_wbuf will be full because server doesn't send data to
            client.

            When s_wbuf is full, server write() will be block and block loop
            which perform read() - write(). Then server won't read data from
            s_rbuf, s_rbuf will be full.

            When s_rbuf is full, server notify client to pause transmiting.
            Then c_wbuf will be full because client doesn't send data to
            server.

            When c_wbuf is full, client write() will be block and block loop 
            which perform write() - read().

        And result is transmiting is block forever, except:
        
            Client read data from c_rbuf.

            Transmiting flow terminates.

    VERIFY

        Copy source file from "tlpi/sockets/is_echo_cl.c" to
        "tlpi/sockets/is_echo_cl_61-1.c". And add program to 
        "tlpi/sockets/Makefile".

        Modify program follow requirement.

USAGE

    Ensure that echo service is started. You can do it by add lines below
    to /etc/inetd.conf. If lines are eraly exists, uncomment it.

    ---/etc/inetd.conf---

    echo            stream  tcp nowait  root    internal
    echo            dgram   upd nowait  root    internal

    ---

    # check state of inetd
    $ systemctl status inetd

    # if inetd doesn't running, start it
    systemctl start inetd
    
    # if inetd running, restart it
    $ killall -s SIGHUP inetd

    # check state of inetd again to ensure it running
    $ systemctl status inetd

    # create small file with readable data
    echo "hello there" > tmp/small-file

    # create big file 100MB 
    $ dd if=/dev/zero of=tmp/big-file bs=1MB count=100

    # put small file to client
    # it work 
    $./tlpi/sockets/is_echo_cl_61-1 < tmp/small-file
    hello there

    # put big file to client
    # client will be block forever
    # press Ctrl + C to terminate
    $ ./tlpi/sockets/is_echo_cl_61-1 < tmp/big-file

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("Goto %s to see solution\n", __FILE__);
    return EXIT_SUCCESS;
}
