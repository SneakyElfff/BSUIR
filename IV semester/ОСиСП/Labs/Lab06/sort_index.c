//
//  sort_index.c
//  Lab06
//
//  Created by Нина Альхимович on 04.06.23.
//

//#include <bits/pthreadtypes.h>
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

typedef struct    //параметры, передаваемые порождаемым потокам
{
    char *buffer;
    int block_size;
    int thread_num;
} ThreadParams;

pthread_barrier_t barrier;
pthread_mutex_t mutex;
int blocks, pairs, *map;

int compare(const void *c1, const void *c2)    //функция сравнения символов
{
    char sym1 = *((char*)c1), sym2 = *((char*)c2);

    if(sym1 < sym2)
        return -1;
    else if(sym1 > sym2)
        return 1;
    else
        return 0;
}


int main(int argc, const char *argv[])
{
    if(argc < 5)
    {
        printf("При запуске программы необходимо указать 4 параметра\n");
        return -1;
    }

    int memsize = atoi(argv[1]), threads = atoi(argv[3]);
    const char *filename = argv[4];
    blocks = atoi(argv[2]);

    if(threads < 8 && threads > 8*8)
    {
        printf("Количество потоков должно быть в диапазоне от k до N, где k - количество ядер, а N - максимум\n");
        return -1;
    }

    if((blocks & (blocks - 1)) && blocks >= threads)
    {
        printf("Количество блоков должно быть степенью двойки и превышать количество потоков\n");
        return -1;
    }

    if(memsize % 4096 != 0)
    {
        printf("Размер рабочего буфера должен быть кратен размеру страницы\n");
        return -1;
    }

    pthread_t *created_threads = (pthread_t *) calloc(threads, sizeof(pthread_t));    //в массиве будут храниться идентификаторы потоков
    if(!created_threads)
    {
        fprintf(stderr, "Память не выделена\n");
        return -1;
    }

    ThreadParams *params = (ThreadParams *) calloc(threads, sizeof(ThreadParams));
    if(!params)
    {
        fprintf(stderr, "Память не выделена\n");
        return -1;
    }

    int file = open(filename, O_RDONLY);
    if(file == -1)
    {
        fprintf(stderr, "Невозможно открыть файл\n");
        return -1;
    }

    char *mapped_file = (char *) mmap(NULL, memsize, PROT_READ, MAP_PRIVATE, file, 0);
    if(mapped_file == MAP_FAILED)
    {
        fprintf(stderr, "Ошибка отображения файла в память\n");
        return -1;
    }

    map = (int *) calloc(blocks, sizeof(int));
    if(!map)
    {
        fprintf(stderr, "Память не выделена\n");
        return -1;
    }

    for(int i=0; i<threads; i++)    //изначально первые блоки считаются занятыми
        map[i] = 1;

    void *threadSort(void *);

    pthread_barrier_init(&barrier, NULL, threads + 1);
    pthread_mutex_init(&mutex, NULL);

    pairs = blocks / 2;

    for(int i=0; i<threads; i++)
    {
        params[i].buffer = mapped_file;
        params[i].block_size = memsize / blocks;
        params[i].thread_num = i + 1;

        if(pthread_create(&created_threads[i], NULL, threadSort, &params[i]))
        {
            fprintf(stderr, "Невозможно создать поток\n");
            exit(EXIT_FAILURE);
        }
    }

    pthread_barrier_wait(&barrier);    //основной поток ожидает потоки на барьере

    ThreadParams main = {mapped_file, memsize / blocks, 0};
    threadSort(&main);

    pthread_barrier_wait(&barrier);

    munmap(mapped_file, memsize);

    for(int i=0; i<threads; i++)
        pthread_cancel(created_threads[i]);

    pthread_mutex_destroy(&mutex);
    //close(file);
    free(map);
    pthread_barrier_destroy(&barrier);
    free(created_threads);
    free(params);
    return 0;
}

void *threadSort(void *arg)
{
    pthread_barrier_wait(&barrier);    //синхронизация потоков на барьере

    ThreadParams *params = (ThreadParams *) arg;
    char *buffer = params->buffer, *block_start, *block1_start, *block2_start;
    int block_size = params->block_size, num = params->thread_num, ind = num, block1, block2;

    void mergeBlocks(char *block1_start, char *block2_start, int block_size);    //функция слияния блоков

    //фаза сортировки

    block_start = buffer + ind * block_size;
    qsort(block_start, block_size, sizeof(char), compare);    //сортировка блока под номером данного потока

    while(ind != blocks && block_start >= buffer + block_size * blocks)    //пока не кончились блоки (если буфер - весь файл) или буфер
    {
        pthread_mutex_lock(&mutex);

        for(int i=ind+1; i<blocks; i++)    //поиск свободного блока
        {
            if(map[i] == 0)
            {
                map[i] = 1;
                ind = i;
                break;
            }
        }

        pthread_mutex_unlock(&mutex);

        block_start = buffer + ind * block_size;
        qsort(block_start, block_size, sizeof(char), compare);

        printf("Блок №%d отсортирован\n", ind);
    }

    pthread_barrier_wait(&barrier);    //когда свободные блоки закончились

    //фаза слияния

    if(num > blocks - 2)    //проверка, существуют ли пары слияния для данного потока
        pthread_barrier_wait(&barrier);

    else
    {
        block1 = num + 1;
        block2 = block1 + 1;

        block1_start = buffer + block1 * block_size;
        block2_start = buffer + block2 * block_size;

        mergeBlocks(block1_start, block2_start, block_size);

        map[block1] = 0;    //отсортированные блоки помечались 1, слитые - 0
        map[block2] = 0;

        pairs--;
    }

    pthread_barrier_wait(&barrier);

    while(pairs > 1)
    {
        pthread_mutex_lock(&mutex);

        for(int i=block2+1; i<blocks; i++)    //поиск свободного блока
        {
            if(map[i] == 1)
            {
                map[i] = 0;
                map[i+1] = 0;
                block1 = i;
                block2 = i + 1;
                break;
            }
        }

        pthread_mutex_unlock(&mutex);

        block1_start = buffer + block1 * block_size;
        block2_start = buffer + block2 * block_size;

        mergeBlocks(block1_start, block2_start, block_size);

        pairs--;

        printf("Блоки №%d и №%d слиты\n", block1, block2);
    }

    if(pairs == 1 && num == 0)
    {
        pthread_mutex_lock(&mutex);

        for(int i=block2+1; i<blocks; i++)    //поиск последней пары блоков
        {
            if(map[i] == 1)
            {
                map[i] = 0;
                map[i+1] = 0;
                block1 = i;
                block2 = i + 1;
                break;
            }
        }

        pthread_mutex_unlock(&mutex);

        block1_start = buffer + block1 * block_size;
        block2_start = buffer + block2 * block_size;

        mergeBlocks(block1_start, block2_start, block_size);

        pairs--;

        printf("Последние блоки №%d и №%d слиты\n", block1, block2);

        pthread_barrier_wait(&barrier);
    }

    else
    {
        pthread_barrier_wait(&barrier);
        return NULL;
    }

    return NULL;
}

void mergeBlocks(char *block1_start, char *block2_start, int block_size)
{
    char *merged_block = (char *) calloc(2 * block_size, sizeof(char));
    if(!merged_block)
    {
        fprintf(stderr, "Память не выделена\n");
        return;
    }

    char *merged_pos = merged_block, *block1_ptr = block1_start, *block2_ptr = block2_start;

    while(block1_ptr < block1_start + block_size && block2_ptr < block2_start + block_size)
    {
        if(*block1_ptr <= *block2_ptr)
        {
            *merged_pos = *block1_ptr;
            block1_ptr++;
        }

        else
        {
            *merged_pos = *block2_ptr;
            block2_ptr++;
        }

        merged_pos++;
    }

    while(block1_ptr < block1_start + block_size)
    {
        *merged_pos = *block1_ptr;
        block1_ptr++;
        merged_pos++;
    }

    while(block2_ptr < block2_start + block_size)
    {
        *merged_pos = *block2_ptr;
        block2_ptr++;
        merged_pos++;
    }
}
