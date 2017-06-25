/*
SYNOPSIS

    Solution for exercise 27-01.

REQUIREMENT

    The final command in the following shell session uses the program in 
    listing 27-3 to exec the program zyz. What happens?.

        $ echo $PATH
        /usr/local/bin:/usr/bin:/bin:./dir1:./dir2
        $ ls -l dir1
        total 8
        -rw-r--r--  1 mtk   users   7860 Jun 13 11:55 xyz
        $ ls -l dir2
        total 28
        -rwxr-xr-x  1 mtk   users   27452 Jun 13 11:55 xyz
        $ ./t_execlp xyz

SOLUTION

    Unimplemented

USAGE

    $ ./dest/bin/27-01

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    // show information to user
    printf("this is solution for exercise 27-1\n");
    printf("see %s for solution\n", __FILE__);

    // success
    return EXIT_SUCCESS;
}
