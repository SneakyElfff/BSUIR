//
//  main.c
//  Lab03
//
//  Created by Нина Альхимович on 11.04.23.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>

void sigusr1_handler(int sig, siginfo_t *info, void *context)
{
    kill(info->si_pid, SIGUSR2);
}

void sigusr2_handler(int sig, siginfo_t *info, void *context)
{
    printf("Дочерний процесс %d завершил вывод статистики\n", info->si_pid);
}

int main(int argc, const char * argv[])
{
    pid_t pid, *children;
    int counter = 0, child_status, num;
    char keystroke, *choice, buf[2];
    
    fd_set set_of_descriptors;    //набор дескрипторов файлов, на которые ожидаются события
    FD_ZERO(&set_of_descriptors);    //очистить набор дескрипторов
    FD_SET(0, &set_of_descriptors);    //добавить stdin
    struct timeval timer = {5, 0};    //установка времени ожидания
    
    void killAll(int *, int *);
    void killAny(int *, int *, int);
    void forbidAll(int *, int);
    void permitAll(int *, int);
    void permitAny(int *, int);
    
    signal(SIGUSR1, sigusr1_handler);    //замена стандартного обработчика
    signal(SIGUSR2, sigusr2_handler);
    
    children = (pid_t *) calloc(1, sizeof(pid_t));
    if(!children)
    {
        fprintf(stderr, "\nПамять не выделена");
        return -1;
    }
    
    while(1)
    {
        puts("\nинформация о расположении: + - создать дочерний процесс;");
        puts("                           - - удалить последний дочерний процесс;");
        puts("                           l - вывести перечень процессов;");
        puts("                           k - удалить все дочерние процессы;");
        puts("                           s - запретить вывод статистики всем;");
        puts("                           g - разрешить вывод статистики всем;");
        puts("                           s<num> - запретить вывод статистики;");
        puts("                           g<num> - разрешить вывод статистики;");
        puts("                           p<num> - запретить вывод статистики всем и запросить у одного;");
        puts("                           q - удалить все дочерние процессы и завершить работу.");
        
        choice = (char *) calloc(8, sizeof(char));
        if(!choice)
        {
            fprintf(stderr, "\nПамять не выделена");
            return -1;
        }
        
        fgets(choice, 8, stdin);
        choice[strcspn(choice, "\n")] = '\0';
        keystroke = choice[0];
        
        if(strlen(choice) == 1)
        {
            switch(keystroke)
            {
                case '+':
                    pid = fork();
                    
                    if(pid == 0)    //код порожденного процесса
                    {
                        execve("./child", NULL, NULL);
                    }

                    else if(pid == -1)
                    {
                        fprintf(stderr, "Ошибка, код ошибки - %d\n", errno);
                        exit(errno);
                    }

                    else    //код родительского процесса
                    {
                        printf("Дочерний процесс %d создан\n", pid);
                        
                        counter++;
                        
                        children = (pid_t *) realloc(children, sizeof(pid_t) * counter);
                        if(!children)
                        {
                            fprintf(stderr, "\nПамять не выделена");
                            return -1;
                        }
                        
                        children[counter-1] = pid;
                    }
                    
                    break;
                    
                case '-':
                    killAny(children, &counter, counter);
                    
                    break;
                    
                case 'l':
                    if(!counter)
                    {
                        printf("Дочерние процессы не были порождены\n");
                        break;
                    }
                    
                    printf("Перечень всех процессов:\n");
                    for(int i=0; i<counter; i++)
                        printf("%d. Процесс %d;\n", i+1, children[i]);
                    
                    break;
                    
                case 'k':
                    killAll(children, &counter);
                    
                    break;
                    
                case 's':
                    forbidAll(children, counter);
                    
                    break;
                    
                case 'g':
                    permitAll(children, counter);
                    
                    break;
                    
                case 'q':
                    killAll(children, &counter);
                    return 0;
                    
                default:
                    fprintf(stderr, "Неверный параметр\n");
                    return -1;
            }
        }
        
        else
        {
            if(choice[1] == '<' && choice[strlen(choice) - 1] == '>')
            {
                switch(choice[0])
                {
                    case 's':
                        sscanf(choice, "s<%d>", &num);
                        
                        if(counter < num)    //проверка на наличие процесса под указанным номером
                        {
                            fprintf(stderr, "Неверный параметр\n");
                            break;
                        }
                            
                        else kill(children[num-1], SIGUSR1);
                        
                        break;
                        
                    case 'g':
                        sscanf(choice, "g<%d>", &num);
                        
                        if(counter < num)
                        {
                            fprintf(stderr, "Неверный параметр\n");
                            break;
                        }
                        
                        else permitAny(children, num);
                        
                        break;
                        
                    case 'p':
                        sscanf(choice, "p<%d>", &num);
                        
                        forbidAll(children, counter);
                        
                        if(counter < num)
                        {
                            fprintf(stderr, "Неверный параметр\n");
                            break;
                        }
                        
                        switch(select(1, &set_of_descriptors, NULL, NULL, &timer))    //NULL - не отслеживать события на запись или возникновение ошибок
                        {
                            case -1:
                                fprintf(stderr, "Ошибка, код ошибки - %d\n", errno);
                                exit(errno);
                                
                            case 1:
                                fgets(buf, sizeof(buf), stdin);
                                
                                if(buf[0] == 'g')
                                    permitAny(children, num);
                                
                                else    //если был введен иной символ
                                    permitAll(children, counter);
                                    
                                break;
                                
                            case 0:    //если время истекло
                                permitAll(children, counter);
                                
                                break;
                        }
                        
                        break;
                        
                    default:
                        fprintf(stderr, "Неверный параметр\n");
                        return -1;
                }
            }
        }
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
            waitpid(children[num-1], &child_status, 0);    //для избежания превращения дочернего процесса в "зомби"
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
        
        printf("Осталось %d дочерних процессов\n", *counter);
    }
}

void forbidAll(int *children, int counter)
{
    if(!counter)
    {
        printf("Дочерние процессы не были порождены\n");
        return;
    }

    for(int i=0; i<counter; i++)
    {
        kill(children[i], SIGUSR1);
        sleep(1);
    }
}

void permitAll(int *children, int counter)
{
    int child_status;
    
    if(!counter)
    {
        printf("Дочерние процессы не были порождены\n");
        return;
    }

    for(int i=0; i<counter; i++)
    {
        kill(children[i], SIGUSR2);
        sleep(1);
    }
    
    pause();
}

void permitAny(int *children, int num)
{
    int child_status;
    
    kill(children[num-1], SIGUSR2);
    sleep(1);
    
    pause();
}
