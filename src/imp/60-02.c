/*
SYNOPSIS

    Solution for exercise 60-02.

REQUIREMENT

    Sometimes, it may be necessary to write a socket server so that it can be
    invoked either directly from the command line or indirectly via inetd. In
    this case, a command-line option is used to distinguish the two cases.
    Modify the program in Example 60-4 so that, if it is given a -i
    command-line option, it assumes that it is being invoked by inetd and
    handles a single client on the connected socket, which inetd supplies via
    STDIN_FILENO. If the -i option is not supplied, then the program can
    assume it is being invoked from the command line, and operate in the
    usual fashion. (This change requires only the addition of a few lines of
    code.) Modify /etc/inetd.conf to invoke this program for the echo service.

SOLUTION

    Copy source file from "tlpi/sockets/is_echo_sv.c" to
    "tlpi/sockets/is_echo_sv_60-2.c" then add program to
    "tlpi/sockets/Makefile".

    In case program was called by inetd, program no need to create socket,
    bind it to specific service, become daemon and accept new peers.
    In simple, program use STDIN_FILENO as peer socket.

    In case program called from command line, perform usual operations.

USAGE

    In case use inetd, modify or add a line to /etc/inetd.conf to help
    inetd invoke program. Replace ABS-PATH below with absolute path to
    tlpiexer directory.

    ---/etc/inetd.conf---

    echo stream tcp nowait root <ABS-PATH/tlpi/sockets/is_echo_sv_60-2> -i

    Then send SIGHUP to inetd to inetd read /etc/inetd.conf again and start
    program

    $ killall -s SIGHUP inetd

    Try few request to localhost:7 which address echo service serve.

    After that, importance remove line just added in /etc/inetd.conf.

    If nomral call program from command line, doesn't specific -i argument.

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
