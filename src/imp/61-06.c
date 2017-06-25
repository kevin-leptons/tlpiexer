/*
SYNOPSIS

    Solution for excercise 61-06.

REQUIREMENT

    Section 61.13.1 noted that an alternative to out-of-band data would be to
    create two socket connections between the client and server: one for
    normal data and one for priority data. Write client and server programs
    that implement this framework. Here are a few hints:

        The server needs some way of knowing which two sockets belong to
        the same client. One way to do this is to have the client first
        create a listening socket using an ephemeral port (i.e., binding to
        port 0). After obtaining the ephemeral port number of its listening
        socket (using getsockname()), the client connects its “normal” socket
        to the server’s listening socket and sends a message containing port
        number of the client’s listening socket. The client then waits for
        the server to use the client’s listening socket to make a connection
        in the opposite for the “priority” socket. (The server can obtain
        the client’s IP address during the accept() of the normal connection.)

        Implement some type of security mechanism to prevent a rogue process
        from to connect to the client’s listening socket. To do this, the
        client could a cookie (i.e., some type of unique message) to the
        server using the normal socket. The server would then return this
        cookie via the priority socket so that the client could verify it.

        In order to experiment with transmitting normal and priority data
        from the client to the server, you will need to code the server to
        multiplex the input from the two sockets using select() or poll()
        (described in Section 63.2).

SOLUTION

   Same as exercise's hints, use poll().

   Key use to client authenticate connection from server is 128 bits.
   It means that program MUST use memcmp() instead of strcmp().

   Each loop, server poll two sockets. First socket is normal socket, use
   to transfer data. Second socket is urgent data socket. If data is available
   on urgent socket, server prefer to handle it's data than normal socket.

   Client handshake with server. After done, it creates child process
   to send data to normal socket. Main process wait for a moment then send
   CLOSE message to server. Server close both normal socket and urgent socket,
   client can't send any data to server anymore.

USAGE

    # start server in background, put stdout/stderr to file
    $ ./dest/bin/61-06-server 8080 &> tmp/server.log &
    [1] 25628

    # start client
    $ ./dest/bin/61-06-client localhost 8080
    Urgent socket: 53591
    Local => Server => Normal socket
    Key => Normal socket
    Urgent socket <= Local <= Server
    Urgent socket <= Key
    Data => Urgent socket: cmd=OK, data=N/A
    Data => Urgent socket: cmd=DATA, data=URGENT URGENT URGENT
    Data => Normal socket: NORMAL NORMAL NORMAL
    Data => Normal socket: NORMAL NORMAL NORMAL
    Data => Normal socket: NORMAL NORMAL NORMAL
    Data => Urgent socket: CLOSE
    Data => Normal socket: ERROR: Server socket was closed
    Data => Normal socket: ERROR: Server socket was closed
    Data => Normal socket: ERROR: Server socket was closed

    # see server log
    $ cat tmp/server.log
    Ready
    Normal socket <= Server <= Client: 39838
    Key <= Normal socket
    Server => 192.168.1.3:54012 => Urgent socket
    Key => Urgent socket
    Urgent socket <= Key
    Urgent socket <= Client: cmd=DATA, data=URGENT URGENT URGENT
    Normal socket <= Client: NORMAL NORMAL NORMAL
    Normal socket <= Client: NORMAL NORMAL NORMAL
    Normal socket <= Client: NORMAL NORMAL NORMAL
    Urgent socket <= Client: cmd=CLOSE

    # stop server
    $ kill %1

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
