//
//  main.c
//  Lab01
//
//  Created by Нина Альхимович on 23.02.23.

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

#define max_file_len 15
#define max_file_path 1024

void dirwalk(char *current, char *choice)
{
    char path_name[max_file_path];
    struct dirent *dir_entry = NULL;    //элемент каталога
    DIR *dir = NULL;
    struct stat buffer;

    if((dir = opendir(current)) == NULL)
    {
        fprintf(stderr, "Невозможно открыть данный каталог %s: %s\n", current, strerror(errno));
        return;
    }

    while((dir_entry = readdir(dir)) != NULL)
    {
        if(strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0)
            continue;    //пропустить данный каталог и родительский

        if(strlen(current) + strlen(dir_entry->d_name) + 2 > sizeof(path_name))
            fprintf(stderr, "Слишком длинное имя: %s %s\n", current, dir_entry->d_name);

        else
        {
            strcpy(path_name, current);    //дополняется путь
            strcat(path_name, "/");
            strcat(path_name, dir_entry->d_name);
        }

        if(lstat(path_name, &buffer) == 0)    //получение информации о файле
        {
            if(S_ISDIR(buffer.st_mode))
            {
                if(strstr(choice, "d") != NULL)    //если получена информация о каталоге
                    printf("directory %s\n", path_name);

                dirwalk(path_name, choice);    //продолжается обход по обнаруженному каталогу
            }

            else if(S_ISREG(buffer.st_mode))
            {
                if(strstr(choice, "f") != NULL)    //если получена информация об обычном файле
                    printf("file %s\n", path_name);
            }

            else if(S_ISLNK(buffer.st_mode))
            {
                if(strstr(choice, "l") != NULL)    //если получена информация о символической ссылке
                    printf("link %s\n", path_name);
            }

            else
            {
                printf("Error\n");
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[])
{
    char current[max_file_path], choice[max_file_path] = "";
    int result = 0;

    opterr = 0;
    while((result = getopt(argc, argv, "ldf")) != -1)
    {
        switch(result)
        {
            case 'l':
                strcat(choice, "l");
                break;
            case 'd':
                strcat(choice, "d");
                break;
            case 'f':
                strcat(choice, "f");
                break;
            case '?':
                printf("Неизвестный параметр\n");
                break;

            default:
                strcpy(current, ".");
                strcat(choice, "ldf");
                break;
        }
    }

    for(int i=optind; i<argc; i++)    //если встречен неизвестный аргумент
        {
            if(strstr(argv[i], "-l") == NULL && strstr(argv[i], "-d") == NULL && strstr(argv[i], "-f") == NULL)    //если это не параметр
            {
                if(strstr(current, "") != NULL)
                    strcpy(current, argv[i]);
            }
            else strcat(choice, argv[i]);
        }

    //установка параметра по умолчанию, если необходимо
    if(strcmp(current, "") == 0) strcat(current, ".");
    if(strcmp(choice, "") == 0) strcat(choice, "ldf");

    dirwalk(current, choice);

    return 0;
}
