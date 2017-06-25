/*
SYNOPSIS

    Part of solution for exercise 54-01. Goto 54-01.c for more information.

AUTHORS

    Kevin Leptons <kevin.leptons@gmail.com>
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>

#include <tlpiexer/54-01.h>
#include <tlpiexer/error.h>

static sem_t *sem_read = NULL;
static sem_t *sem_write = NULL;
static int shm_fd = 0;
static struct shared_mem *shm = NULL;
static ssize_t rdone = 0;

static void clean_up();
static void term_handle(int sig);

int main(int argc, char *argv[])
{
    if (signal(SIGINT, term_handle) == SIG_ERR)
        goto EXIT_ERROR;
    if (signal(SIGTERM, term_handle) == SIG_ERR)
        goto EXIT_ERROR;

    sem_read = sem_open(SEM_READ_NAME, 0);
    if (sem_read == SEM_FAILED)
        goto EXIT_ERROR;
    sem_write = sem_open(SEM_WRITE_NAME, 0);
    if (sem_write == SEM_FAILED)
        goto EXIT_ERROR;
    shm_fd = shm_open(SHARED_MEM_NAME, O_RDONLY, 0);
    if (shm_fd < 0)
        goto EXIT_ERROR;
    shm = mmap(NULL, sizeof(*shm), PROT_READ, MAP_SHARED, shm_fd, 0);
    if (shm == MAP_FAILED)
        goto EXIT_ERROR;

    for (;;) {
        if (sem_wait(sem_write) != FN_OK)
            goto EXIT_ERROR;
        rdone += shm->size;
        errno = 0;
        write(STDOUT_FILENO, shm->data, shm->size); 
        if (errno != 0)
            goto EXIT_ERROR;
        if (sem_post(sem_read) != FN_OK)
            goto EXIT_ERROR;
        if (shm->size == 0)
            break;
    }

    clean_up();
    return EXIT_SUCCESS;

EXIT_ERROR:
    perror("Error");
    clean_up();
    return EXIT_FAILURE;
}

static void clean_up()
{
    if (sem_read != NULL)
        sem_close(sem_read);
    if (sem_write != NULL)
        sem_close(sem_write);
    if (shm != NULL)
        munmap(shm, sizeof(*shm));
    if (shm_fd > 0)
        close(shm_fd);
}

static void term_handle(int sig)
{
    clean_up();
    signal(sig, SIG_DFL);
    kill(getpid(), sig);
}
