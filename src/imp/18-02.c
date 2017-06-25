/*
SYNOPSIS

    Solution for exercise 18-02.

REQUIREMENT

    Why does the call to chmod() in the following code fail?

        mkdir("test", S_IRUSR | S_IWUSR | S_IXUSR);
        chdir("test");
        fd = open("myfile", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        symlink("myfile", "../mylink");
        chmod("../mylink", S_IRUSR);

SOLUTION

    Unimplemented

USAGE

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
