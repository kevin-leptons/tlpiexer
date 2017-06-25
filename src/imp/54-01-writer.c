/*
SYNOPSIS

    Part of solution for exercise 54-01. Goto 54-01.c for more information.

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h> 
#include <stdio.h> 
#include <sys/mman.h> 
#include <sys/stat.h> 
#include <sys/types.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>

#include <tlpiexer/54-01.h>
#include <tlpiexer/error.h>

static sem_t *sem_read = NULL;
static sem_t *sem_write = NULL;
static int shm_fd = 0;
static struct shared_mem *shm = NULL;
static ssize_t wdone = 0;

static void clean_up(void);
static void term_handle(int sig);

int main(int argc, char *argv[])
{
    if (argc > 1)
        exit_usage("Not accept any arguments");

    if (signal(SIGINT, term_handle) == SIG_ERR)
        goto EXIT_ERROR;
    if (signal(SIGTERM, term_handle) == SIG_ERR)
        goto EXIT_ERROR;

    sem_read = sem_open(SEM_READ_NAME, O_CREAT | O_EXCL, S_IRWXU, 1);
    if (sem_read == NULL)
        goto EXIT_ERROR;
    sem_write = sem_open(SEM_WRITE_NAME, O_CREAT | O_EXCL, S_IRWXU, 0);
    if (sem_write == NULL)
        goto EXIT_ERROR;
    shm_fd = shm_open(SHARED_MEM_NAME, O_CREAT | O_EXCL | O_RDWR, S_IRWXU);
    if (shm_fd == FN_ER)
        goto EXIT_ERROR;

    // now, sem_write is blocked, reader can't do anything
    // so initialize of mem_fd is safe
    if (ftruncate(shm_fd, sizeof(*shm)) != FN_OK)
        goto EXIT_ERROR;
    shm = mmap(NULL, sizeof(*shm), 
               PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm == MAP_FAILED)
        goto EXIT_ERROR;

    for (;;) {
        if (sem_wait(sem_read) != FN_OK)
            goto EXIT_ERROR;
        errno = 0;
        shm->size = read(STDIN_FILENO, shm->data, sizeof(shm->data));
        if (errno != 0)
            goto EXIT_ERROR;
        wdone += shm->size;
        printf("Written %lu bytes\n", shm->size);
        if (sem_post(sem_write) != FN_OK)
            goto EXIT_ERROR;
        if (shm->size == 0)
            break;
    }

    if (sem_wait(sem_read) != FN_OK)
        goto EXIT_ERROR;

    clean_up();
    return EXIT_SUCCESS;

EXIT_ERROR:
    perror("Error");
    clean_up();
    return EXIT_FAILURE;
}

static void clean_up(void)
{
    printf("Written total %lu bytes\n", wdone);

    if (sem_read != NULL)
        sem_close(sem_read);
    if (sem_write != NULL)
        sem_close(sem_write);
    if (shm != NULL)
        munmap(shm, sizeof(*shm));
    if (shm_fd > 0)
        close(shm_fd);

    sem_unlink(SEM_READ_NAME);
    sem_unlink(SEM_WRITE_NAME);
    shm_unlink(SHARED_MEM_NAME);
}

static void term_handle(int sig)
{
    clean_up();
    signal(sig, SIG_DFL);
    kill(getpid(), sig);
}
