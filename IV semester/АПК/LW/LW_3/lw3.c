//
//  LW_3.c
//  LW_3
//
//  Created by Нина Альхимович on 27.03.23.
/*  1. Запрограммировать второй канал таймера таким образом, чтобы динамик компьютера издавал звуки.
       Для всех каналов таймера:
 считать слово состояния и вывести его на экран в двоичной форме;
 рассчитать коэффициент деления (значение счетчика CE) и вывести его на экран в шестнадцатеричной форме.
 */

#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <stdlib.h>

void setCE(int);    //функция установки значения счётчика
void getChannelInfo(void);

int main()
{
    setCE(659);    //загрузить CE канала нужным значением, 659 Гц

    outp(0x61, inp(0x61) | 3);    //включение динамика, 2 младших бита - в 1
    delay(4000);    //задержка, в течение которой проигрывается нота
    outp(0x61, inp(0x61) & 0xfc);    //выключение динамика, 2 младших бита - в 0
    
    getChannelInfo();    //чтение слов состояния каналов

    return 0;
}

void setCE(int frequency)
{
    short value = 1193180 / frequency;    //вычисляем значение для регистра счетчика таймера, 1193182 - частота таймера
    
    outp(0x43, 0xB6);    //вывести в порт регистра 43h управляющее слово: 10110110b, канал 2, операция 4, режим 3, формат 0

    outp(0x42, (char)value);    //загрузка младшего байта
    outp(0x42, (char)(value >> 8));    //загрузка старшего байта
}

void getChannelInfo()
{
    unsigned char temp;
    char *result;

    result = (char*) calloc(9, sizeof(char));
    if(!result)
    {
        printf("\nMemory wasn't allocated");
        return;
    }

    //0 канал
    printf("CHANNEL 0:\n");
    
    outp(0x43, 0xe2);    //11100010b
    temp = inp(0x40);    //считывание слова состояния канала (RBC + #)
    itoa(temp, result, 2);
    printf("State: %s\n", result);
    
    outp(0x43, 0x00);    //команда СLC (код - 0x00)
    temp = inp(0x40);    //младший байт счетчика
    temp = (inp(0x40) << 8) + temp;    //старший байт
    itoa(temp, result, 16);
    printf("Counter's register: %s\n", result);
    
    //1 канал
    printf("CHANNEL 1:\n");
    
    outp(0x43, 0xe4);    //11100100b
    temp = inp(0x41);
    itoa(temp, result, 2);
    printf("State: %s\n", result);
    
    outp(0x43, 0x40);
    temp = inp(0x41);
    temp = (inp(0x41) << 8) + temp;
    itoa(temp, result, 16);
    printf("Counter's register: %s\n", result);

    //2 канал
    printf("CHANNEL 2:\n");
    
    outp(0x43, 0xe8);    //11101000b
    temp = inp(0x42);
    itoa(temp, result, 2);
    printf("State: %s\n", result);
    
    outp(0x43, 0x80);
    temp = inp(0x42);
    temp = (inp(0x42) << 8) + temp;
    itoa(temp, result, 16);
    printf("Counter's register: %s\n", result);

    free(result);
}
