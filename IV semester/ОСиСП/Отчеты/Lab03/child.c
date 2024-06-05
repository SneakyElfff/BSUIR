//
//  child.c
//  Lab03
//
//  Created by Нина Альхимович on 11.04.23.
//

//#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#define iterations 101

struct Data
{
    int x;
    int y;
} pair;

void alternation(void)
{
    static int temp = 0;
    
    if(temp % 4 == 0)
    {
        pair.x = 0;
        pair.y = 0;
    }
    
    else if(temp % 4 == 1)
    {
        pair.x = 0;
        pair.y = 1;
    }
    
    else if(temp % 4 == 2)
    {
        pair.x = 1;
        pair.y = 0;
    }
    
    else
    {
        pair.x = 1;
        pair.y = 1;
    }
    
    temp++;
}

volatile int output_allowed = 0;

void sigusr1_handler(int sig)
{
    output_allowed = 0;    //вывод статистики запрещен
}

void sigusr2_handler(int sig)
{
    output_allowed = 1;
}

int main(int argc, const char *argv[], char *env[])
{
    struct timespec alarm = {0, 1000000}, left;    //структура, содержащая количество секунд и наносекунд для задержки
    pair.x = 0;
    pair.y = 0;
    int i =0, counter_of_pairs[4] = {0, 0, 0, 0};
    
    signal(SIGUSR1, sigusr1_handler);
    signal(SIGUSR2, sigusr2_handler);
    
//    printf("Дочерний процесс %d просит разрешения\n", getpid());
    
    kill(getppid(), SIGUSR1);    //запросить разрешение у родительского процесса на вывод статистики
    
    do
    {
        nanosleep(&alarm, &left);
        
        while(1)
        {
            alternation();
            
            while(nanosleep(&alarm, &left) == -1)
            {
                if(errno == EINTR)
                {
                    fprintf(stderr, "Ошибка, код ошибки - %d\n", errno);
                    alarm = left;
                }
                
                else
                {
                    fprintf(stderr, "Ошибка, код ошибки - %d\n", errno);
                    exit(errno);
                }
            }
            
            if(nanosleep(&alarm, &left) == 0)
            {
                if(pair.x == 0 && pair.y == 0)
                    counter_of_pairs[0]++;
                else if (pair.x == 0 && pair.y == 1)
                    counter_of_pairs[1]++;
                else if (pair.x == 1 && pair.y == 0)
                    counter_of_pairs[2]++;
                else if (pair.x == 1 && pair.y == 1)
                    counter_of_pairs[3]++;
                
                break;
            }
        }
        
        i++;
        
        if(i == 101)
        {
//            pause();    //ожидать ответа от родителя
            
            while(!output_allowed)
            {
                printf("Дочернему процессу %d вывод статистики запрещен\n", getpid());
                
                pause();
            }
            
            if(output_allowed)
            {
                
                printf("\nИдентификатор родительского процесса: %d\n", getppid());
                printf("Идентификатор дочернего процесса: %d\n", getpid());
                printf("Статистика: {0,0} - %d, {0,1} - %d, {1,0} - %d, {1,1} - %d\n", counter_of_pairs[0], counter_of_pairs[1], counter_of_pairs[2], counter_of_pairs[3]);
                
//                printf("Дочерний процесс %d завершил вывод статистики\n", getpid());
                
                kill(getppid(), SIGUSR2);    //сообщить родительскому процессу, что вывод завершен
            }
            
            i = 0;
        }
    } while(i<iterations);
    
    exit(0);
}
