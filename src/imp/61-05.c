/*
SYNOPSIS

    Solution for exercise 61-05.

REQUIREMENT

    Write a client and a server that permit the cilent to execute arbitrary
    shell commands on the server host (If you don't implement any security
    mechanism in this application, you should ensure that the server is
    operating under a user account where it can do no damage if invoked by
    malicious users). The client should be executed with two command-line
    arguments.

        $ ./is_shell_cl server-host 'some-shell-command'

    After connecting to the server, the client sends the given command to
    the server, and then closes its writing half of the socket using
    shutdown(), so that the server sees end-of-file. The server should handle
    each incoming connection, the server should read the command from the
    socket (until end-of-file), and then exec a shell to perform the command.
    Here are a couple hints:

        See the implementation of system() in Section 27.7 fro an example
        of how to execute a shell command.

        By using dup2() to duplicate the socket on standard ouput and
        standard error, the execed command will automatically write to the
        socket.

SOLUTION

    SIGIN

        Create interface called signin(const  char *usr, const char *pass) to
        set user id, group id of current proces to specific username.

        It calls getpwnam(), getspname(), crypt() to verify username and
        password. If username and password is correct, perform:

            setresuid() to change real effective, saved user id.
            setfsuid() to change file will be create to user id.
            setfsgid() to change file will be create to group id.

        Design and implementation put into "include/tlpiexer/61-5.h" and
        "src/imp/61-5-server.c".

    PROTOCOL

        msg_req contains usrname and password fields. It also contains command
        is invoked from client.

        msg_res simple is string.

        Desgin put into "include/tlpiexer/61-5.h".

    SERVER

        Create a socket to concurent accept new connection from client.
        With each connection from client, create child process then call
        req_handle() to process it. req_hanle() process follow.

        Read request to get username and password.

        Call signin(username, passowrd) to change to specific user.

        Create child process:

            Redirect ouput from stdout and stderr to client socket descriptor
            by call dup2() for this purpose.

            Replace child process with shell program to execute command by
            call execl("/bin/sh", "sh", "-c", command, (char*) 0).

        On parent process:

            Wait for child process terminate.

            Close client socket.

        Implementation put into "src/imp/61-5-server.c"

    CLIENT

        Create msg_req depend on command line arguments.

        Connect to server.

        Write msg_req.

        Read response from server and display to stdout.

        Implementation put into "src/imp/61-5-client.c"

USAGE

    # start server
    # it requires privilege permission to switch to specific user
    $ sudo ./dest/bin/61-05-server
    Password:
    
    # create files
    $ ./dest/bin/61-05-client localhost kevin <passwd> "mkdir -vp tmp"
    $ ./dest/bin/61-05-client localhost kevin <passwd> "echo hello > tmp/kevin"
    $ ./dest/bin/61-05-client localhost root <passwd> "echo hello > tmp/root"

    # list files
    $ ./dest/bin/61-05-client localhost kevin <passwd> "ls -la tmp"
    total 16
    drwxr-xr-x  2 kevin root  4096 Apr 24 23:54 .
    drwxr-xr-x 14 kevin kevin 4096 Apr 24 23:53 ..
    -rw-r--r--  1 kevin root     6 Apr 24 23:54 kevin
    -rw-r--r--  1 root  root     6 Apr 24 23:54 root

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("Gto %s to see solution\n", __FILE__);
    return EXIT_SUCCESS;
}
