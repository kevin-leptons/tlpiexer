/*
SYNOPSIS

    Solution for excercise 27-03.

REQUIREMENT 

    What ouput would we see if we make the following script executable
    and exec() it?.

        #!/bin/cat -n
        hello world

SOLUTION

    That script invoke cat program and give content of file as input.

USAGE

    # create file with content and make it executable
    cat > tmp/hello.cat << EOF
    > #!/bin/cat -n
    > hello world
    > EOF
    $ chmod +x tmp/hello.cat

    $ ./tmp/hello.cat
         1 #!/bin/cat -n
         2 hello world

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
