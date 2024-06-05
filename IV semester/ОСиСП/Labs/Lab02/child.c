//
//  child.c
//  Lab02
//
//  Created by Нина Альхимович on 23.03.23.
//

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, const char *argv[], char *env[])
{
    char *temp;
    size_t len = 500;
    temp = (char *) calloc(len, sizeof(char));
    if(!temp)
    {
        fprintf(stderr, "\nПамять не выделена");
        return -1;
    }

    fprintf(stdout, "\nДочерний процесс %s стартовал\n", argv[0]);
    printf("Идентификатор родительского процесса: %d\n", getppid());
    printf("Идентификатор дочернего процесса: %d\n\n", getpid());

    FILE *f = fopen(argv[1], "r");
    if(f == NULL)
    {
        fprintf(stderr, "\nНевозможно открыть файл");
        return -1;
    }
    else
    {
        while(getline(&temp, &len, f) != -1)    //считывание очередной строки (имя переменной) из файла
        {
            temp[strcspn(temp, "\n")] = '\0';    //удаление символа \n

                if(!getenv(temp))
                {
                    fprintf(stderr, "Параметр %s не найден\n", temp);
                    continue;
                }
            printf("%s=%s\n", temp, getenv(temp));
        }
    }

    fclose(f);
    exit(0);
}
