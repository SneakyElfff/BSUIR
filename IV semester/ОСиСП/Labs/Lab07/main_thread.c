//
//  main_thread.c
//  Lab07
//
//  Created by Нина Альхимович on 06.06.23.
//

#define _POSIX_C_SOURCE 200809L

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <pthread.h>
#define shm_key 5678

#pragma pack(1)    //директива для выравнивания структуры по 1 байту

typedef struct
{
    unsigned char type;
    unsigned short hash;
    unsigned short size;
    unsigned char padding;    //наполнитель, если необходимо выровнять размер до кратности 4 байтам
    unsigned char data[256];
} Message;

typedef struct
{
    Message **messages;
    Message **head;
    Message **tail;
    unsigned int count_added;
    unsigned int count_extracted;
} Queue;

Queue *queue;
sem_t sem_add, sem_extract;
pthread_mutex_t access_mutex;
int count_messages = 0, array_size = 100;
pthread_cond_t prod_cond;    //условные переменные
pthread_cond_t cons_cond;

int main(int argc, const char *argv[])
{
    int count_prod = 0, count_cons = 0;
    pthread_t pthread, *producers, *consumers;

    void *producer();
    void *consumer();
    void killAll(pthread_t *, int *);
    void killAny(pthread_t *, int *, int, int);

    producers = (pthread_t *) calloc(1, sizeof(pthread_t));
    if(!producers)
    {
        fprintf(stderr, "\nПамять не выделена");
        return -1;
    }

    consumers = (pthread_t *) calloc(1, sizeof(pthread_t));
    if(!consumers)
    {
        fprintf(stderr, "\nПамять не выделена");
        return -1;
    }

    queue = (Queue *) calloc(1, sizeof(Queue));
    if(!queue)
    {
        fprintf(stderr, "\nПамять не выделена");
        return -1;
    }

    if(sem_init(&sem_add, 0, 1) == -1)
    {
        fprintf(stderr, "Ошибка инициализации семафора\n");
        exit(EXIT_FAILURE);
    }

    if(sem_init(&sem_extract, 0, 1) == -1)
    {
        fprintf(stderr, "Ошибка инициализации семафора\n");
        exit(EXIT_FAILURE);
    }

    if(pthread_mutex_init(&access_mutex, NULL) != 0)
    {
        fprintf(stderr, "Ошибка инициализации мьютекса\n");
        exit(EXIT_FAILURE);
    }

    if(pthread_cond_init(&prod_cond, NULL) != 0)
    {
        fprintf(stderr, "Ошибка инициализации условной переменной\n");
        exit(EXIT_FAILURE);
    }

    if(pthread_cond_init(&cons_cond, NULL) != 0)
    {
        fprintf(stderr, "Ошибка инициализации условной переменной\n");
        exit(EXIT_FAILURE);
    }

    queue->messages = (Message **) calloc(array_size, sizeof(Message *));
    if(!queue->messages)
    {
        fprintf(stderr, "\nПамять не выделена");
        return -1;
    }
    queue->head = &(queue->messages[0]);
    queue->tail = &(queue->messages[0]);
    queue->count_added = 0;
    queue->count_extracted = 0;

    printf("Выберите действие:\n");
    printf("                  c - создать поток-производитель;\n");
    printf("                  + - создать поток-потребитель;\n");
    printf("                  d - закрыть поток-производитель;\n");
    printf("                  - - закрыть поток-потребитель;\n");
    printf("                  > - увеличить размер очереди;\n");
    printf("                  < - уменьшить размер очереди;\n");
    printf("                  q - завершить работу.\n");

    while(1)
    {
        switch(getchar())
        {
            case 'c':
                if(pthread_create(&pthread, NULL, producer, NULL))
                {
                    fprintf(stderr, "Невозможно создать поток\n");
                    exit(EXIT_FAILURE);
                }

                printf("Поток-производитель %ld создан\n", pthread);

                count_prod++;

                producers = (pthread_t *) realloc(producers, sizeof(pthread_t) * count_prod);
                if(!producers)
                {
                    fprintf(stderr, "\nПамять не выделена");
                    return -1;
                }

                producers[count_prod-1] = pthread;

                printf("Итого: %d производителей\n", count_prod);

                break;

            case '+':
                if(pthread_create(&pthread, NULL, consumer, NULL))
                {
                    fprintf(stderr, "Невозможно создать поток\n");
                    exit(EXIT_FAILURE);
                }

                printf("Поток-потребитель %ld создан\n", pthread);

                count_cons++;

                consumers = (pthread_t *) realloc(consumers, sizeof(pthread_t) * count_cons);
                if(!consumers)
                {
                    fprintf(stderr, "\nПамять не выделена");
                    return -1;
                }

                consumers[count_cons-1] = pthread;

                printf("Итого: %d потребителей\n", count_cons);

                break;

            case 'd':
                killAny(producers, &count_prod, count_prod, 1);

                break;

            case '-':
                killAny(consumers, &count_cons, count_cons, 0);

                break;

            case '>':
                array_size++;

                queue->messages = (Message **) realloc(queue->messages, sizeof(Message *) * array_size);
                if(!queue->messages)
                {
                    fprintf(stderr, "\nПамять не выделена");
                    return -1;
                }

                printf("Размер очереди увеличен с %d до %d\n", array_size-1, array_size);

                //printf("Дальнейшее увеличение размеров очереди невозможно\n");

                break;

            case '<':
                if(array_size != 1)
                {
                    array_size--;

                    queue->messages = (Message **) realloc(queue->messages, sizeof(Message *) * array_size);
                    if(!queue->messages)
                    {
                        fprintf(stderr, "\nПамять не выделена");
                        return -1;
                    }

                    printf("Размер очереди уменьшен с %d до %d\n", array_size+1, array_size);
                }

                else printf("Дальнейшее уменьшение размеров очереди невозможно\n");

                break;

            case 'q':
                killAll(producers, &count_prod);
                killAll(consumers, &count_cons);

                free(producers);
                free(consumers);
                free(queue->messages);
                free(queue);
                sem_destroy(&sem_add);
                sem_destroy(&sem_extract);
                pthread_mutex_destroy(&access_mutex);
                pthread_cond_destroy(&prod_cond);
                pthread_cond_destroy(&cons_cond);

                return 0;

            default:
                fprintf(stderr, "Неверный параметр\n");
                return -1;
        }

        getchar();
    }

    return 0;
}

void *producer()
{
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

        sem_wait(&sem_add);    //уменьшить счетчик семафоров

        pthread_mutex_lock(&access_mutex);    //захватить мьютекс

        if(count_messages >= array_size)
        {
            fprintf(stderr, "Очередь переполнена. Ожидание потребителя и перемещение в начало...\n\n");
            while(count_messages == array_size)    //ожидать освобождения очереди
                pthread_cond_wait(&prod_cond, &access_mutex);
            count_messages = 0;
            queue->tail = &(queue->messages[0]);    //перезапись элементов буфера сначала
        }

        queue->messages[(queue->count_added) % array_size] = message;
        queue->tail = &(queue->messages[(queue->count_added) % array_size]);

        queue->count_added++;

        count_messages++;

        if(count_messages == 1)    //если очередь была пуста, но теперь доступна для извлечения
            pthread_cond_signal(&cons_cond);

        printf("%u-ое сообщение добавлено в очередь\n", queue->count_added);
        printf("Контрольные данные: %u\n", message->hash);
        printf("Длина данных: %u\n", message->size);
        printf("Данные сообщения: ");
        for(int i=0; i<((size + 3)/4)*4; i++)
            printf("%u ", message->data[i]);
        printf("\n\n");

        pthread_mutex_unlock(&access_mutex);    //освободить мьютекс

        sem_post(&sem_add);

        sleep(1);
    }
}

void *consumer()
{
    while (1)
    {
        sem_wait(&sem_extract);

        pthread_mutex_lock(&access_mutex);

        if(count_messages <= 0)
        {
            fprintf(stderr, "Очередь пуста. Ожидание производителя...\n");

            while(count_messages <= 0)    //ожидать заполнения очереди
                pthread_cond_wait(&cons_cond, &access_mutex);
        }

        queue->tail = &(queue->messages[(queue->count_added - 2) % array_size]);
        queue->messages[(queue->count_added - 1) % array_size] = NULL;

        queue->count_extracted++;

        count_messages--;

        if(count_messages == array_size - 1)    //если очередь была переполнена, но теперь доступна для заполнения
            pthread_cond_signal(&prod_cond);

        printf("%u сообщений извлечено из очереди\n\n", queue->count_extracted);

        pthread_mutex_unlock(&access_mutex);

        sem_post(&sem_extract);

        sleep(1);
    }
}

void killAll(pthread_t *children, int *counter)
{
    pthread_t pthread;
    int child_status;

    if(!*counter)
    {
        printf("Потоки не были созданы\n");
        return;
    }

    else
    {
        for(int i=0; i<*counter; i++)
        {
            if(pthread_cancel(children[i]) != 0)
            {
                fprintf(stderr, "Ошибка закрытия потока %ld\n", children[i]);
                continue;
            }

            else
            {
                printf("Поток %ld закрыт\n", children[i]);
                pthread_join(children[i], NULL);
            }
        }

        *counter = 0;

        children = (pthread_t *) realloc(children, sizeof(pthread_t));
        if(!children)
        {
            fprintf(stderr, "\nПамять не выделена");
            return;
        }
    }

    printf("Все потоки закрыты\n");
}

void killAny(pthread_t *children, int *counter, int num, int type)
{
    pthread_t pthread;
    int child_status;

    if(!*counter)
    {
        printf("Потоки не были открыты\n");
        return;
    }

    else
    {
        if(pthread_cancel(children[num-1]) != 0)
        {
            fprintf(stderr, "Ошибка закрытия потока %ld\n", children[num-1]);
        }

        else
        {
            printf("Поток %ld закрыт\n", children[num-1]);
            pthread_join(children[num-1], NULL);
        }

        for(int i=num-1; i<*counter; i++)
            children[i] = children[i+1];    //перезапись массива идентификаторов

        (*counter)--;

        if(!*counter) children = (pthread_t *) realloc(children, sizeof(pthread_t));    //если был удален единственный элемент
        else children = (pthread_t *) realloc(children, sizeof(pthread_t) * (*counter));
        if(!children)
        {
            fprintf(stderr, "\nПамять не выделена");
            return;
        }

        if(count_messages == array_size && type)    //если поток-производитель был закрыт во время ожидани освобождения очереди
        {
            pthread_mutex_unlock(&access_mutex);

            sem_post(&sem_add);
        }

        if(count_messages == 0 && !type)    //если поток-потребитель был закрыт во время ожидани заполнения очереди
        {
            pthread_mutex_unlock(&access_mutex);

            sem_post(&sem_extract);
        }

        printf("Осталось %d потоков\n\n", *counter);
    }
}
