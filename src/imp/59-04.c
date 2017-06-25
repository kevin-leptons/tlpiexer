/*
SYNOPSIS

    Solution for excercise 59-04.

REQUIREMENT

    Write a network server that stores name-value pairs. The server should
    allow names to be added, deleted, modified, and retrieved by clients.
    Write one or more client programs to test the server. Optionally,
    implement some kind of security mechanism that allows only the client
    that created the name to delete it or to modify the value associated with
    it.

SOLUTION

    LINKING LIST

        Design item_nv structure, it contains fields such as name, value, own.

        Desgin linking list item, it contains fields such as item, next, prev.
        item store an item_nv structure. Next, prev point to next and prev
        item.

        Design linking list APIs to add, del, get items.

        Desgin auth APIs to verfiy operation with linking list.

        All of design put into "include/tlpiexer/59-04-llist.h" and implement
        in "src/lib/59-04-llist.c".

    PROTOCOL

        Design msg_req structure, it contains command in ADD, DEL, GET, SET
        to invoke server. It also contains item.

        Design msg_res structure, it contains result in OK, PERM, ERROR. It
        also
        contains item.

        Design isock_connect(), isock_listen() to connect, listen on internet
        socket domain.

        All of design put into "include/tlpiexer/59-04-isock.h" and implement
        in "src/lib/59-04-isock.c"

    SERVER

        Use internet socket domain, stream type to communicate with client.
        Implement in "src/imp/59-04-server.c".

    CLIENT

        Use internet socket domain, stream type to communicate with server.
        Implement in "src/imp/59-04-client.c".

USAGE

    # start server in background
    $ ./dest/bin/59-04-server &

    # try few of commands
    $ ./dest/bin/59-04-client add name-1 value-1
    Success
    $ ./dest/bin/59-04-client get name-1
    Success
    name=name-1; value=value-1; own=kevin
    $ ./dest/bin/59-04-client set name-1 value-11
    Success
    $ ./dest/bin/59-04-client get name-1
    Success
    name=name-1; value=value-11; own=kevin

    # change to other user and try commands
    # it must warn permision with commands update, remove
    $ su
    Password:
    $ ./dest/bin/59-04-client get name-1
    Success
    name=name-1; value=value-11; own=kevin
    $ ./dest/bin/59-04-client set name-1 value-111
    Permision denied
    $ ./dest/bin/59-04-client del name-1
    Permision denied
    $ exit

    # back to own user and remove name
    $ ./dest/bin/59-04-client del name-1
    Success

    # try remove one again, it must warn name isn't exist
    $ ./dest/bin/59-04-client del name-1
    Item doesn't exist

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
