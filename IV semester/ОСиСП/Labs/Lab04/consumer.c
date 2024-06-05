#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#define queue_length 100
#define sem_key 1234
#define shm_key 5678

#define _POSIX_C_SOURCE 200809L

#pragma pack(1)

typedef struct
{
    unsigned char type;
    unsigned short hash;
    unsigned short size;
    unsigned char padding;
    unsigned char data[256];
} Message;

typedef struct
{
    Message *messages[queue_length];
    Message **head;
    Message **tail;
    unsigned int count_added;
    unsigned int count_extracted;
} Queue;

int main(int argc, const char *argv[], char *env[])
{
    int shm_fd = shm_open("/message_queue", O_RDWR, 0666);
    if(shm_fd == -1)
    {
        fprintf(stderr, "Невозможно получить разделяемую память\n");
        exit(EXIT_FAILURE);
    }

    Queue *queue = (Queue *) mmap(NULL, sizeof(Queue), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(queue == MAP_FAILED)
    {
        fprintf(stderr, "Ошибка отображения разделяемой памяти\n");
        exit(EXIT_FAILURE);
    }

    sem_t *sem_extract = sem_open("/extract", O_RDWR);
    if(sem_extract == SEM_FAILED)
    {
        fprintf(stderr, "Невозможно открыть семафор\n");
        exit(EXIT_FAILURE);
    }

    int shm_fd_mutex = shm_open("/access", O_RDWR, 0666);
    if(shm_fd_mutex == -1)
    {
        fprintf(stderr, "Невозможно получить разделяемую память для мьютекса\n");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_t *access = (pthread_mutex_t *)mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_mutex, 0);
    if(access == MAP_FAILED)
    {
        fprintf(stderr, "Ошибка отображения разделяемой памяти для мьютекса\n");
        exit(EXIT_FAILURE);
    }

    int shm_fd_counter = shm_open("/counter", O_RDWR, 0666);
    if(shm_fd_counter == -1)
    {
        fprintf(stderr, "Невозможно получить разделяемую память\n");
        exit(EXIT_FAILURE);
    }

    int *counter = (int *) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_counter, 0);
    if(counter == MAP_FAILED)
    {
        fprintf(stderr, "Ошибка отображения разделяемой памяти\n");
        exit(EXIT_FAILURE);
    }

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(access, &attr);

    while (1)
    {
        sem_wait(sem_extract);

        pthread_mutex_lock(access);

        printf("SKZ: %d\n", *counter);
        if(*counter <= 0)
        {
            fprintf(stderr, "Очередь пуста\n");

            pthread_mutex_unlock(access);

            sem_post(sem_extract);

            while(*counter <= 0)
                continue;
        }

        queue->tail = &(queue->messages[(queue->count_added - 2) % queue_length]);
        queue->messages[(queue->count_added - 1) % queue_length] = NULL;

        queue->count_extracted++;

        (*counter)--;

        pthread_mutex_unlock(access);

        sem_post(sem_extract);

        printf("%u сообщений извлечено из очереди\n\n", queue->count_extracted);

        sleep(1);
    }

    if(munmap(queue, sizeof(Queue)) == -1)
    {
        fprintf(stderr, "Ошибка закрытия разделяемой памяти\n");
        exit(EXIT_FAILURE);
    }

    if(sem_close(sem_extract) == -1)
    {
        fprintf(stderr, "Ошибка закрытия семафора\n");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_destroy(access);
    if(munmap(access, sizeof(pthread_mutex_t)) == -1)
    {
        fprintf(stderr, "Ошибка уничтожения мьютекса\n");
        exit(EXIT_FAILURE);
    }

    if(munmap(counter, sizeof(int)) == -1)
    {
        fprintf(stderr, "Ошибка закрытия разделяемой памяти\n");
        exit(EXIT_FAILURE);
    }

    exit(0);
}
