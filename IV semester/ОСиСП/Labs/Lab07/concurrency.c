#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#define num_of_records 10

typedef struct
{
    char name[80];
    char address[80];
    uint8_t semester;
} record_s;

record_s last_rec, records[num_of_records];
int last_num;

struct flock lock;

int main(int argc, const char *argv[])
{
    const char *filename = "data.bin";
    int rec_num;
    record_s record;

    void printRecords(const char *);
    record_s getRecord(const char *, int);
    void modifyRecord(const char *, int);
    void saveRecord(const char *, record_s);

    printRecords(filename);

    while(1)
    {
        printf("\nВыберите действие:\n");
        printf("                  1 - LST (вывести содержимое файла);\n");
        printf("                  2 - GET (получить запись по номеру);\n");
        printf("                  3 - MOD (модифицировать поля записи);\n");
        printf("                  4 - PUT (сохранить последнюю запись);\n");
        printf("                  0 - завершить работу.\n");

        switch(getchar())
        {
            case '1':
                printRecords(filename);

                break;

            case '2':
                printf("Введите номер записи: ");
                scanf("%d", &rec_num);
                if(rec_num < 0 || rec_num >= num_of_records)
                {
                    printf("Записи с таким номером не существует\n");
                    break;
                }

                getRecord(filename, rec_num);

                break;

            case '3':
                printf("Введите номер записи, которую нужно изменить: ");
                scanf("%d", &rec_num);
                if(rec_num < 0 || rec_num >= num_of_records)
                {
                    printf("Записи с таким номером не существует\n");
                    break;
                }

                modifyRecord(filename, rec_num);

                break;

            case '4':
                saveRecord(filename, last_rec);

                printf("Запись %d модифицирована\n", last_num);

                break;

            case '0':
                return 0;

            default:
                fprintf(stderr, "Неверный параметр\n");
                return -1;
        }

        getchar();
    }

    return 0;
}

void printRecords(const char *filename)
{
    int file = open(filename, O_RDONLY);
    if(file == -1)
    {
        fprintf(stderr, "Невозможно открыть файл\n");
        return;
    }

    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    if(fcntl(file, F_SETLKW, &lock) == -1)
    {
        fprintf(stderr, "Ошибка блокировки\n");
        close(file);
        return;
    }

    record_s rec;
    int count_records = 0;

    while(read(file, &rec, sizeof(rec)) > 0)
    {
        printf("Запись №%d:\n", count_records);
        printf("ФИО: %s\n", rec.name);
        printf("Адрес: %s\n", rec.address);
        printf("Семестр: %d\n\n", rec.semester);

        records[count_records] = rec;

        count_records++;
    }

    printf("Вывод завершен\n\n");

    lock.l_type = F_UNLCK;
    fcntl(file, F_SETLK, &lock);
    close(file);
}

record_s getRecord(const char *filename, int rec_num)
{
    int file = open(filename, O_RDONLY);
    if(file == -1)
    {
        fprintf(stderr, "Невозможно открыть файл\n");
        exit(EXIT_FAILURE);
    }

    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = rec_num * sizeof(record_s);
    lock.l_len = sizeof(record_s);

    if(fcntl(file, F_SETLKW, &lock) == -1)
    {
        fprintf(stderr, "Ошибка блокировки\n");
        close(file);
        exit(EXIT_FAILURE);
    }

    record_s rec;

    lseek(file, rec_num * sizeof(record_s), SEEK_SET);
    read(file, &rec, sizeof(record_s));

    printf("ФИО: %s\n", rec.name);
    printf("Адрес: %s\n", rec.address);
    printf("Семестр: %d\n\n", rec.semester);

    lock.l_type = F_UNLCK;
    fcntl(file, F_SETLK, &lock);
    close(file);

    return rec;
}

void modifyRecord(const char *filename, int rec_num)
{
    record_s rec = getRecord(filename, rec_num), temp;

    printf("Заполните поля, требующие модификации:\n");
    printf("ФИО: ");
    getchar();
    fgets(temp.name, sizeof(temp.name), stdin);
    temp.name[strcspn(temp.name, "\n")] = '\0';  // Удаление символа новой строки
    if(strlen(temp.name) > 0)
        strcpy(rec.name, temp.name);

    printf("Адрес: ");
    fgets(temp.address, sizeof(temp.address), stdin);
    temp.address[strcspn(temp.address, "\n")] = '\0';
    if(strlen(temp.address) > 0)
        strcpy(rec.address, temp.address);

    printf("Семестр: ");
    char semester_str[10];
    fgets(semester_str, sizeof(semester_str), stdin);
    semester_str[strcspn(semester_str, "\n")] = '\0';
    if(strlen(semester_str) > 0)
        rec.semester = atoi(semester_str);

    int file = open(filename, O_WRONLY);
    if(file == -1)
    {
        fprintf(stderr, "Невозможно открыть файл\n");
        exit(EXIT_FAILURE);
    }

    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = rec_num * sizeof(record_s);
    lock.l_len = sizeof(record_s);

    if(fcntl(file, F_SETLKW, &lock) == -1)
    {
        fprintf(stderr, "Ошибка блокировки\n");
        close(file);
        exit(EXIT_FAILURE);
    }

    lseek(file, rec_num * sizeof(record_s), SEEK_SET);
    write(file, &rec, sizeof(record_s));

    lock.l_type = F_UNLCK;
    fcntl(file, F_SETLK, &lock);
    close(file);

    last_rec = rec;
    last_num = rec_num;

    printf("\nДля сохранения изменений нажмите Enter и выберите четвертый пункт меню\n");
    getchar();
}

void saveRecord(const char *filename, record_s current)
{
    if(memcmp(&records[last_num], &current, sizeof(record_s)) != 0)
    {
        int file = open(filename, O_RDWR);
        if(file == -1)
        {
            fprintf(stderr, "Невозможно открыть файл\n");
            exit(EXIT_FAILURE);
        }

        memset(&lock, 0, sizeof(lock));
        lock.l_type = F_RDLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = last_num * sizeof(record_s);
        lock.l_len = sizeof(record_s);

        if(fcntl(file, F_SETLKW, &lock) == -1)
        {
            fprintf(stderr, "Ошибка блокировки\n");
            close(file);
            exit(EXIT_FAILURE);
        }

        record_s rec_new;

        lseek(file, last_num * sizeof(record_s), SEEK_SET);
        read(file, &rec_new, sizeof(record_s));

        if(memcmp(&current, &rec_new, sizeof(record_s)) != 0)
        {
            lock.l_type = F_UNLCK;
            fcntl(file, F_SETLK, &lock);
            close(file);

            saveRecord(filename, rec_new);
            return;
        }

        lseek(file, last_num * sizeof(record_s), SEEK_SET);
        write(file, &current, sizeof(record_s));

        lock.l_type = F_UNLCK;
        fcntl(file, F_SETLK, &lock);
        close(file);
    }
}
