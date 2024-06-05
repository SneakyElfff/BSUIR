//
//  parent.c
//  Lab02
//
//  Created by Нина Альхимович on 17.03.23.
//

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <locale.h>
#include <string.h>

extern char **environ;

void sortEnv(char **params, int num)    //Bubble-сортировка параметров среды
{
    char *temp;

    for(int i=0; i<num-1; i++)
        for(int j=num-1; j>i; j--)
            for(int k=0; k<256; k++)
            {
                if(params[j-1][k] == params[j][k])
                    continue;

                else if(params[j-1][k] > params[j][k])
                {
                    temp = params[j-1];
                    params[j-1] = params[j];
                    params[j] = temp;

                    break;
                }

                else break;
            }
}

char *findPath(char **env_params, int num)    //функция, сканирующа массив параметров среды
{
    char *path = NULL;

       for(char **p=env_params; *p; ++p)
       {
           if(strncmp(*p, "CHILD_PATH=", 11) == 0)    //сравнение первых 4 символов текущей строки со строкой "PWD="
           {
               path = *p + 11;    //сохранение в переменную path найденного пути
               break;
           }
       }

       return path;
}

int main(int argc, const char *argv[], char *env[])
{
    setenv("CHILD_PATH", ".", 0);

    setlocale(LC_COLLATE, "C");

    int x = 0, num, child_status;
    char **args, **name;
    pid_t pid;

    fprintf(stdout, "\nРодительский процесс стартовал...\n");

    for(num=0; ; num++)    //подсчет количества переменных среды
        if(env[num] == NULL)
            break;

    args = (char **) calloc(num, sizeof(char *));
    if(!args)
    {
        fprintf(stderr, "\nПамять не выделена");
        return -1;
    }

    for(int i=0; i<num; i++)
    {
        args[i] = (char *) calloc(strlen(env[i]) + 1, sizeof(char));
        if(!args[i])
        {
            fprintf(stderr, "\nПамять не выделена");
            return -1;
        }

        for(unsigned long j=0; j<strlen(env[i]); j++)
            args[i][j] = env[i][j];    //получение значение переменных среды и запись их в массив args
    }

    sortEnv(args, num);    //функция сортировки переменных среды

    for(int i=0; i<num; i++)
        fprintf(stdout, "%s\n", args[i]);

    name = (char **) calloc(3, sizeof(char *));    //массив строк, содержащий имя дочернего процесса и файла
    if(!name)
    {
        fprintf(stderr, "\nПамять не выделена");
        return -1;
    }

    for(int i=0; i<2; i++)
    {
        name[i] = (char *) calloc(50, sizeof(char));
        if(!name[i])
        {
            fprintf(stderr, "\nПамять не выделена");
            return -1;
        }
    }

    char *temp;
    temp = (char *) calloc(12, sizeof(char));
    if(!temp)
    {
        fprintf(stderr, "\nПамять не выделена");
        return -1;
    }

    if(argc != 2)
    {
        fprintf(stderr, "\nНе указано имя файла");
        return -1;
    }
    strcpy(name[1], argv[1]);

    while(x < 100)
    {
        sprintf(temp, "%d", x);    //установка имени программы с инкрементированным номером
        strcpy(name[0], "child_");
        strcat(name[0], temp);

        puts("\nинформация о расположении: +-используя getenv()");
        puts("                           *-сканируя массив параметров среды");
        puts("                           &-сканируя массив параметров, указанный в environ");
        puts("                           q-окончить");

        switch(getchar())
        {
            case '+':
                pid = fork();

                if(pid == 0)
                {
                    execve(strcat(getenv("CHILD_PATH"), "/child"), name, env);

                    exit(1);
                }

                else if(pid == -1)
                {
                    fprintf(stderr, "Ошибка, код ошибки - %d\n", errno);
                    exit(errno);
                }

                else
                {
                    waitpid(pid, &child_status, 0);
                    printf("Дочерний процесс завершился со статусом: %d\n", child_status);
                }

                break;

            case '*':
                pid = fork();

                if(pid == 0)
                {
                    execve(strcat(findPath(env, num), "/child"), name, env);

                    exit(1);
                }

                else if(pid == -1)
                {
                    fprintf(stderr, "Ошибка, код ошибки - %d\n", errno);
                    exit(errno);
                }

                else
                {
                    waitpid(pid, &child_status, 0);
                    printf("Дочерний процесс завершился со статусом: %d\n", child_status);
                }

                break;

            case '&':
                pid = fork();

                if(pid == 0)
                {
                    execve(strcat(findPath(environ, num), "/child"), name, env);

                    exit(1);
                }

                else if(pid == -1)
                {
                    fprintf(stderr, "Ошибка, код ошибки - %d\n", errno);
                    exit(errno);
                }

                else
                {
                    waitpid(pid, &child_status, 0);
                    printf("Дочерний процесс завершился со статусом: %d\n", child_status);
                }

                break;

            case 'q':
                return 0;

            default:
                fprintf(stderr, "Неверный параметр\n");
                return -1;
        }

        getchar();
        x++;
    }

    exit(0);
}
