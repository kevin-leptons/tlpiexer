/*
SYNOPSIS

    Solution for exercise 38-01.

REQUIREMENT

    Login as a normal, unprivileged user, create an executable file (or copy
    an existing file such as /bin/sleep), and enable the set-user-id permission
    bit on that file (chmod u+s). Try modifying the file (e.g., cat >> file).
    What happens to the file permissions as a result (ls -l)? why does this
    happen.

SOLUTION

    $ # copy /bin/sleep to tmp/sleep
    $ cp /bin/sleep tmp/

    $ # enable set-user-id permission
    $ chmod u+s tmp/sleep
    $ ls tmp/sleep
    -rwsr-xr-x 1 kevin kevin 31208 Aug 14 14:01 tmp/sleep

    $ # modify executable file
    $ echo abc >> tmp/sleep

    $ # retrieve file permissions
    $ # set-user-id of file must disapear for security reasons
    $ # if set-uer-id is not disapear, some one can modify it to damge system
    $ ls -l tmp/sleep
    -rwxr-xr-x 1 kevin kevin 31212 Aug 14 14:03 tmp/sleep

USAGE

    $ ./dest/bin/38-01
    Goto ... to see solution

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    printf("Goto %s to see solution\n", __FILE__);
    return EXIT_SUCCESS;
}
