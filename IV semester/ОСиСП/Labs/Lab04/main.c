//
//  main.c
//  Lab04
//
//  Created by Нина Альхимович on 25.05.23.
//

#define _POSIX_C_SOURCE 200809L

#include <unistd.h>
#include <bits/pthreadtypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#define queue_length 100
#define shm_key 5678
#define SHARED_MEM_SIZE sizeof(Queue)

#pragma pack(1)    //директива для выравнивания структуры по 1 байту

typedef struct
{
    unsigned char type;
    unsigned short hash;
    unsigned short size;
    unsigned char padding;    //pаполнитель, если необходимо выровнять размер до кратности 4 байтам
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

int main(int argc, const char *argv[])
{
    char **name;
    int count_prod = 0, count_cons = 0;
    pid_t pid, *producers, *consumers;

    void killAll(int *, int *);
    void killAny(int *, int *, int);

    name = (char **) calloc(3, sizeof(char *));
    if(!name)
    {
        fprintf(stderr, "\nПамять не выделена");
        return -1;
    }

    producers = (pid_t *) calloc(1, sizeof(pid_t));
    if(!producers)
    {
        fprintf(stderr, "\nПамять не выделена");
        return -1;
    }

    consumers = (pid_t *) calloc(1, sizeof(pid_t));
    if(!consumers)
    {
        fprintf(stderr, "\nПамять не выделена");
        return -1;
    }

    printf("Выберите действие:\n");
    printf("                  c - создать процесс-производитель;\n");
    printf("                  + - создать процесс-потребитель;\n");
    printf("                  d - удалить процесс-производитель;\n");
    printf("                  - - удалить процесс-потребитель\n");
    printf("                  q - завершить работу.\n");

    int shm_fd = shm_open("/message_queue", O_CREAT | O_RDWR, 0666);
    if(shm_fd == -1)
    {
        perror("Ошибка при создании/открытии разделяемой памяти");
        exit(EXIT_FAILURE);
    }

    if(ftruncate(shm_fd, sizeof(Queue)) == -1)
    {
        perror("Не удалось изменить размер разделяемой памяти");
        exit(EXIT_FAILURE);
    }

    Queue *queue = mmap(NULL, sizeof(Queue), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(queue == MAP_FAILED)
    {
        perror("Ошибка отображения разделяемой памяти");
        exit(EXIT_FAILURE);
    }

    sem_t *sem_add = sem_open("/add", O_CREAT, 0666, 1);
    if(sem_add == SEM_FAILED)
    {
        perror("Невозможно открыть семафор");
        exit(EXIT_FAILURE);
    }

    sem_t *sem_extract = sem_open("/extract", O_CREAT, 0666, 1);
    if(sem_extract == SEM_FAILED)
    {
        perror("Невозможно открыть семафор");
        exit(EXIT_FAILURE);
    }

    int shm_fd_mutex = shm_open("/access", O_CREAT | O_RDWR, 0666);
    if(shm_fd_mutex == -1)
    {
        perror("Невозможно получить разделяемую память для мьютекса");
        exit(EXIT_FAILURE);
    }

    if(ftruncate(shm_fd_mutex, sizeof(pthread_mutex_t)) == -1)
    {
        perror("Не удалось изменить размер разделяемой памяти для мьютекса");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_t *access = mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_mutex, 0);
    if(access == MAP_FAILED)
    {
        perror("Ошибка отображения разделяемой памяти для мьютекса");
        exit(EXIT_FAILURE);
    }

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);    //инициализация атрибутов мьютекса по умолчанию
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);    //позволяет разным процессам получить доступ к одному и тому же мьютексу
    pthread_mutex_init(access, &attr);

    int shm_fd_counter = shm_open("/counter", O_CREAT | O_RDWR, 0666);
    if(shm_fd_counter == -1)
    {
        perror("Ошибка при создании/открытии разделяемой памяти");
        exit(EXIT_FAILURE);
    }

    if(ftruncate(shm_fd_counter, sizeof(int)) == -1)
    {
        perror("Не удалось изменить размер разделяемой памяти");
        exit(EXIT_FAILURE);
    }

    int *counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_counter, 0);
    if(counter == MAP_FAILED)
    {
        perror("Ошибка отображения разделяемой памяти");
        exit(EXIT_FAILURE);
    }

    for(int i=0; i<queue_length; i++)
        queue->messages[i] = NULL;
    queue->head = &(queue->messages[0]);
    queue->tail = &(queue->messages[0]);
    queue->count_added = 0;
    queue->count_extracted = 0;

    *counter = 0;

    while(1)
    {
        switch(getchar())
        {
            case 'c':
                pid = fork();

                if(pid == 0)    //код порожденного процесса
                {
                    execve("./producer", name, NULL);
                }

                else if(pid == -1)
                {
                    fprintf(stderr, "Ошибка, код ошибки - %d\n", errno);
                    exit(errno);
                }

                else    //код родительского процесса
                {
                    printf("Процесс-производитель %d создан\n", pid);

                    count_prod++;

                    producers = (pid_t *) realloc(producers, sizeof(pid_t) * count_prod);
                    if(!producers)
                    {
                        fprintf(stderr, "\nПамять не выделена");
                        return -1;
                    }

                    producers[count_prod-1] = pid;
                }

                break;

            case '+':
                pid = fork();

                if(pid == 0)
                {
                    execve("./consumer", name, NULL);
                }

                else if(pid == -1)
                {
                    fprintf(stderr, "Ошибка, код ошибки - %d\n", errno);
                    exit(errno);
                }

                else
                {
                    printf("Процесс-потребитель %d создан\n", pid);

                    count_cons++;

                    consumers = (pid_t *) realloc(consumers, sizeof(pid_t) * count_cons);
                    if(!consumers)
                    {
                        fprintf(stderr, "\nПамять не выделена");
                        return -1;
                    }

                    consumers[count_cons-1] = pid;
                }

                break;

            case 'd':
                killAny(producers, &count_prod, count_prod);

                break;

            case '-':
                killAny(consumers, &count_cons, count_cons);

                break;

            case 'q':
                killAll(producers, &count_prod);
                killAll(consumers, &count_cons);

                if(munmap(queue, SHARED_MEM_SIZE) == -1)
                {
                    fprintf(stderr, "Невозможно закрыть разделяемую память\n");
                    exit(EXIT_FAILURE);
                }

                if(shm_unlink("/message_queue") == -1)
                {
                    fprintf(stderr, "Ошибка закрытия разделяемой памяти\n");
                    exit(EXIT_FAILURE);
                }

                if(sem_close(sem_add) == -1)    //закрыть семафор
                {
                    fprintf(stderr, "Невозможно закрыть семафор\n");
                    exit(EXIT_FAILURE);
                }

                if(sem_unlink("/add") == -1)
                {
                    fprintf(stderr, "Ошибка закрытия семафора\n");
                    exit(EXIT_FAILURE);
                }

                if(sem_close(sem_extract) == -1)
                {
                    fprintf(stderr, "Невозможно закрыть семафор\n");
                    exit(EXIT_FAILURE);
                }

                if(sem_unlink("/extract") == -1)
                {
                    fprintf(stderr, "Ошибка закрытия семафора\n");
                    exit(EXIT_FAILURE);
                }

                pthread_mutex_destroy(access);
                if(munmap(access, sizeof(pthread_mutex_t)) == -1)
                {
                    fprintf(stderr, "Невозможно уничтожить мьютекс\n");
                    exit(EXIT_FAILURE);
                }

                if(munmap(counter, SHARED_MEM_SIZE) == -1)
                {
                    fprintf(stderr, "Невозможно закрыть разделяемую память\n");
                    exit(EXIT_FAILURE);
                }

                if(shm_unlink("/counter") == -1)
                {
                    fprintf(stderr, "Ошибка закрытия разделяемой памяти\n");
                    exit(EXIT_FAILURE);
                }
                return 0;

            default:
                fprintf(stderr, "Неверный параметр\n");
                return -1;
        }

        getchar();
    }

    return 0;
}

void killAll(int *children, int *counter)
{
    pid_t pid;
    int child_status;

    if(!*counter)
    {
        printf("Дочерние процессы не были порождены\n");
        return;
    }

    else
    {
        for(int i=0; i<*counter; i++)
        {
            pid = kill(children[i], SIGKILL);

            if(pid != 0)
            {
                fprintf(stderr, "Дочерний процесс %d уже удален\n", children[i]);
                continue;
            }

            else
            {
                printf("Дочерний процесс %d удален\n", children[i]);
                waitpid(children[i], &child_status, 0);    //для избежания превращения дочернего процесса в "зомби"
            }
        }

        *counter = 0;

        children = (pid_t *) realloc(children, sizeof(pid_t));
        if(!children)
        {
            fprintf(stderr, "\nПамять не выделена");
            return;
        }
    }

    printf("Все дочерние процессы удалены\n");
}

void killAny(int *children, int *counter, int num)
{
    pid_t pid;
    int child_status;

    if(!*counter)
    {
        printf("Дочерние процессы не были порождены\n");
        return;
    }

    else
    {
        pid = kill(children[num-1], SIGKILL);

        if(pid != 0)
        {
            fprintf(stderr, "Дочерний процесс %d уже удален\n", children[num-1]);
        }

        else
        {
            printf("Дочерний процесс %d удален\n", children[num-1]);
            waitpid(children[num-1], &child_status, 0);
        }

        for(int i=num-1; i<*counter; i++)
            children[i] = children[i+1];    //перезапись массива идентификаторов

        (*counter)--;

        if(!*counter) children = (pid_t *) realloc(children, sizeof(pid_t));    //если был удален единственный элемент
        else children = (pid_t *) realloc(children, sizeof(pid_t) * (*counter));
        if(!children)
        {
            fprintf(stderr, "\nПамять не выделена");
            return;
        }

        printf("Осталось %d дочерних процессов\n\n", *counter);
    }
}
