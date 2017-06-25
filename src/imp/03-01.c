/*
SYNOPSIS

    Solution for exercise 03-01.

REQUIREMENT

    When using the Linux-specific reboot() system call to reboot the system, 
    the second argument, magic2, must be specified as one of a set of magic
    numbers (e.g., LINUX_REBOOT_MAGIC2). What is the significance of these
    numbers? (Converting them to hexadecimal provides clue).

SOLUTION

    As "man 2 reboot", reboot() will fail if magic2 isn't specific follow
    values:

        LINUX_REBOOT_MAGIC1
        LINUX_REBOOT_MAGIC2
        LINUX_REBOOT_MAGIC2A
        LINUX_REBOOT_MAGIC2B
        LINUX_REBOOT_MAGIC2C

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/reboot.h>

int main(int argc, char *argv[])
{
    printf("LINUX_REBOOT_MAGIC1: 0x%X\n", LINUX_REBOOT_MAGIC1);
    printf("LINUX_REBOOT_MAGIC2: 0x%X\n", LINUX_REBOOT_MAGIC2);
    printf("LINUX_REBOOT_MAGIC2A: 0x%X\n", LINUX_REBOOT_MAGIC2A);
    printf("LINUX_REBOOT_MAGIC2B: 0x%X\n", LINUX_REBOOT_MAGIC2B);
    printf("LINUX_REBOOT_MAGIC2C: 0x%X\n", LINUX_REBOOT_MAGIC2C);
    return EXIT_SUCCESS;
}
