/*
SYNOPSIS

    Solution for exercise 80-01.

REQUIREMENT

    When we execute the following code, we find that it display the same number
    twice, even though the two users have different ids in the password file.
    Why is this?

        printf("%ld %ld\n", (long) (getpwnam("avr")->pw_uid),
               (long) (getpwnam("tsr")->pw_uid));

SOLUTION

    getpwdnam() return static address to struct passwd. It mean that last
    calling to getpwnam() set value of static address to "tsr" user. This
    is reason printf()  two same values.

USAGE

    $ ./dest/bin/08-01
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
