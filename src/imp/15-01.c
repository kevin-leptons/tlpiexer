/*
SYNOPSIS

    Solution for exercise 15-01.

REQUIREMENT

    Section 15.4 contained several statments about the permissions required
    for various file-system operations. use shell commands or write programs 
    to verify or answer the following

     a) removing all owner permissions from a file denies the file owner 
        access, even though group and other do have access

     b) on a directory with read permission but not execute permission,
        the names of files in the directory can be listed, but the files 
        themselves can't be accessed, regardless of the permission on them

     c) what permissions are required on the parent directory and the file 
        itself in order to create a new file, open a file for reading, open a
        file for writing, and delete a file? what permission are required on
        the source and target directory to rename a file? if the target file
        of a rename operation already sticky permission bit (chmod +t) of a
        directory affect renaming and deletion operations?

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
