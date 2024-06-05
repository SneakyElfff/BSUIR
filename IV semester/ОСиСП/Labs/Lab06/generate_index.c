#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

struct index_s    //структура индексной записи
{
    double time_mark;    //временная метка (модифицированная юлианская дата)
    uint64_t recno;    //первичный индекс в таблице БД
} index_record;

struct index_hdr_s    //структура заголовка
{
    uint64_t records;    //количество записей
    struct index_s idx[];    //массив записей в количестве records
} index_header;

int main(int argc, const char *argv[])
{
    if(argc < 3)
    {
        printf("При запуске программы необходимо указать имя файла и размер индекса\n");
        return -1;
    }

    const char *filename = argv[1];
    uint64_t num = strtoull(argv[2], NULL, 10), primary_ind = 1;
    FILE *file;

    double generateTimeMark();

    if(num % 256 != 0)
    {
        printf("Размер индекса в записях должен быть кратен 256\n");
        return -1;
    }

    struct index_hdr_s *header = (struct index_hdr_s *) calloc(1, sizeof(struct index_hdr_s) + num * sizeof(struct index_s));
    if(!header)
    {
        fprintf(stderr, "Память не выделена\n");
        return -1;
    }

    header->records = num;

    srand(time(NULL));    //инициализировать генератор случайных чисел

    for(uint64_t i=0; i<header->records; i++)    //заполнение записей
    {
        header->idx[i].time_mark = generateTimeMark();
        header->idx[i].recno = primary_ind;
        primary_ind++;
    }

    file = fopen(filename, "w");
    if(!file)
    {
        fprintf(stderr, "Невозможно открыть файл\n");
        return -1;
    }

    fwrite(header, sizeof(struct index_hdr_s) + num * sizeof(struct index_s), 1, file);

    printf("Индексный файл сгенерирован\n");

    fclose(file);

    free(header);

    return 0;
}

double generateTimeMark()
{
    int past = 15020, current = 60098;    //01.01.1900, 04.06.2023

    int random_integer = rand() % (current - past) + past;
    double random_fraction = (double)rand() / RAND_MAX;

    return random_integer + random_fraction;
}
