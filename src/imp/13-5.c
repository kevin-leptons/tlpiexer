// SPEC     : solution for exercise 13-5
//
// REQUIRE  :
//
// the command ail [-n num] file prints the last num lines (ten by default)
// of the named file. implement this command using I/O system call (lseek(),
// read(), write(), and so on). keep in mind the buffering issues described in
// this chapter, in order to make the implementation efficient
//
// SOLUTION :
//
// implement showline()
//
//  - open file
//  - seek to end of file
//  - read from end of file to begin of file until get n line. put data to 
//    buffer
//  - show content of buffer to stdout
//
// implement main()
//
//  - switch argument to call showline() with arguments
//
// USAGE    :
//
// <cmd> [-n num] file
// display last num lines of file, ten by default
//
// <cmd> -h
// show help information
//
// AUTHOR   : kevin.leptons@gmail.com

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define RET_SUCCESS 0
#define RET_FAILURE -1
#define TAIL_NUMLINE 10
#define BUF_SIZE 1024

// spec     : show help information to stdout
// arg      : none
// ret      : none
void showhelp();

// spec     : display last n line of file
// arg      :
//  - filepath: path to file will be display
//  - numline: number of line to display
// ret      :
//  - 0 on success
//  - -1 on error
int showline(char *filepath, size_t numline);

int main(int argc, char **argv)
{
    // variables
    size_t numline;
    char opt;

    // switch operation depend on arguments
    if (argc == 2) {

        // format: tail -h
        // display help
        if (strcmp(argv[1], "-h") == 0)
            showhelp();

        // format: tail file
        // display last TAIL_NUMLINE of file
        else
            showline(argv[1], TAIL_NUMLINE);
    }

    // format: tail -n n file
    // display lat n line of file
    else if (argc == 4) {

        while ((opt = getopt(argc, argv, "n:")) != -1) {
            if (opt == 'n') {
                numline = atol(optarg);
                if (numline == 0)
                    return EXIT_SUCCESS;
            } else {
                return EXIT_FAILURE;
            }
        }

        if (optind > argc) {
            fprintf(stderr, "expect file argument\n");
            return EXIT_FAILURE;
        }

        showline(argv[optind], numline);
    }

    // unknow command
    else
        showhelp();

    // success
    return EXIT_SUCCESS;
}

int showline(char *filepath, size_t numline)
{
    // variables
    int fd;
    char buf[BUF_SIZE];
    size_t rsize;
    off_t offset, preoff;
    int numlf;
    int i;

    // init
    numlf = 0;

    // open
    fd = open(filepath, O_RDONLY);
    if (fd == -1)
        return EXIT_FAILURE;

    // seek to end of line
    if ((preoff = lseek(fd, 0, SEEK_END)) == -1)
        return EXIT_FAILURE;

    // find first position of last n line
    // todo: make stament more simple
    while (true) {
        offset = preoff > BUF_SIZE ? preoff - BUF_SIZE : 0;
        if (lseek(fd, offset, SEEK_SET) == -1)
            return EXIT_FAILURE;

        if ((rsize = read(fd, buf, preoff - offset)) == -1)
           return EXIT_FAILURE;     

        if (lseek(fd, offset, SEEK_SET) == -1)
            return EXIT_FAILURE;

        preoff = offset;

        for (i = rsize - 1; i >= 0; i--) {
            if (buf[i] == '\n')
                numlf += 1;
            if (numlf > numline) {
                offset += i + 1;
                break;
            }
        }

        if (offset == 0 || numlf > numline)
            break;

    }

    // verify process of finding
    if (offset == -1)
        return EXIT_FAILURE;

    // seek to start of last n line
    if (lseek(fd, offset, SEEK_SET) == -1)
        return EXIT_FAILURE;

    // display
    while ((rsize = read(fd, buf, BUF_SIZE)) > 0) {
        buf[rsize] = '\0';
        printf("%s", buf);
    }

    // close
    if (close(fd) == -1)
        return EXIT_FAILURE;

    // verify process of display
    if (rsize == -1)
        return EXIT_FAILURE;

    // success
    return EXIT_SUCCESS;
}

void showhelp()
{
    // variables
    char exename[] = "13-5";

    printf("\nUSAGE:\n\n");
    printf("%s [-n numline] file\n", exename);
    printf("%s -h\n\n", exename);

    printf("OPTIONS:\n\n");
    printf("-n: number of last lines will show. default it 10\n\n"); 
}
