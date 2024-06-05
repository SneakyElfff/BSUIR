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
    int shm_fd = shm_open("/message_queue", O_RDWR, 0666);    //открыть разделяемую память
    if(shm_fd == -1)
    {
        fprintf(stderr, "Невозможно получить разделяемую память\n");
        exit(EXIT_FAILURE);
    }

    Queue *queue = (Queue *) mmap(NULL, sizeof(Queue), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);    //отобразить разделяемую память в виртуальное адресное пространство
    if(queue == MAP_FAILED)
    {
        fprintf(stderr, "Ошибка отображения разделяемой памяти\n");
        exit(EXIT_FAILURE);
    }

    sem_t *sem_add = sem_open("/add", O_RDWR);
    if(sem_add == SEM_FAILED)
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

    srand(time(NULL));    //инициализировать генератор случайных чисел

    while (1)
    {
        Message *message = (Message *) calloc(1, sizeof(Message));
        if(!message)
        {
            fprintf(stderr, "\nПамять не выделена");
            exit(EXIT_FAILURE);
        }
        message->type = 1;
        message->hash = 0;

        unsigned short size = rand() % 257;

        if(size == 0)
            continue;

        if(size == 256)
        {
            message->size = 0;
            size = 256;
        }

        else
            message->size = size;

        for(int i=0; i<((size + 3)/4)*4; i++)
        {
            message->data[i] = rand() % 256;
            message->hash += message->data[i];
        }

        sem_wait(sem_add);    //уменьшить счетчик семафоров

        pthread_mutex_lock(access);    //захватить мьютекс

        if(*counter >= queue_length)
        {
            fprintf(stderr, "Очередь переполнена. Перемещение в начало...\n");
            sleep(2);
            *counter = 0;
            queue->tail = &(queue->messages[0]);    //перезапись элементов буфера сначала
        }

        queue->messages[(queue->count_added) % queue_length] = message;
        queue->tail = &(queue->messages[(queue->count_added) % queue_length]);

        queue->count_added++;

        (*counter)++;

        pthread_mutex_unlock(access);    //освободить мьютекс

        sem_post(sem_add);

        printf("%u-ое сообщение добавлено в очередь\n", queue->count_added);
        printf("Контрольные данные: %u\n", message->hash);
        printf("Длина данных: %u\n", message->size);
        printf("Данные сообщения: ");
        for(int i=0; i<((size + 3)/4)*4; i++)
            printf("%u ", message->data[i]);
        printf("\n\n");

        sleep(1);
    }

    if(munmap(queue, sizeof(Queue)) == -1)
    {
        fprintf(stderr, "Ошибка закрытия разделяемой памяти\n");
        exit(EXIT_FAILURE);
    }

    if(sem_close(sem_add) == -1)
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
