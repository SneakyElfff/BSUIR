//
//  LW_5.c
//  LW_5
//
//  Created by Нина Альхимович on 24.04.23.
/*  Осуществить мигание индикаторами клавиатуры.
    Условия реализации программы, необходимые для выполнения лабораторной работы:
 1. Запись байтов команды должна выполняться только после проверки незанятости входного регистра контроллера клавиатуры. Проверка осуществляется считыванием и анализом регистра состояния контроллера клавиатуры;
 2. Для каждого байта команды необходимо считывать и анализировать код возврата. В случае считывания кода возврата, требующего повторить передачу байта, необходимо повторно выполнить передачу байта. При этом повторная передача данных не исключает выполнения всех оставшихся условий;
 3. Для определения момента получения кода возврата необходимо использовать аппаратное прерывания от клавиатуры;
 4. Все коды возврата должны быть выведены на экран в шестнадцатеричной форме. */

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <conio.h>
#include <io.h>
#define n 3    //количество миганий

int quit = 0, resend = 1, counter = 0;

void blink(int);
void controlLED(unsigned char);

void interrupt(*system_9)(void);    //указатель на прерывание
void interrupt custom_9();

int main(int argc, const char * argv[])
{
    int i;
    
    system_9 = getvect(0x09);    //сохранить указатель на старый обработчик
    
    setvect(0x09, custom_9);    //установить новый обработчик прерываний
    
    system("cls");
    printf("Press any key (ESC - to quit the program):\n");
    
    while(!quit)
    {
        blink(1);
        delay(500);
        
        blink(0);
        delay(500);
    }
    
    setvect(0x09, system_9);    //восстановить старый обработчик прерываний
    
    printf("Pressed keystrokes: ");
    
    return 0;
}

void interrupt custom_9()
{
    unsigned char value;
    char *result;

    result = (char *) calloc(9, sizeof(char));
    if(!result)
    {
        printf("\nMemory wasn't allocated");
        return;
    }

    value = inp(0x60);    //считать скан-код клавиши из порта ввода/вывода данных клавиатуры

    if(value == 0x01)    //если нажата клавиша ESC
        quit = 1;

    if(value != 0xFA)    //если нет подтверждения успешного выполнения команды
    {
        itoa(value, result, 16);
        printf("Return code: %s\n", result);
        
        if(counter == 2)
        {
            printf("\n");
            counter = 0;
        }
        counter++;
        
        resend = 1;
    }
    
    else resend = 0;

    free(result);

    (*system_9)();
}

void blink(int indicator)
{
    switch(indicator)
    {
        case 1:
            controlLED(0x07);    //включить индикатор клавиш CapsLock, ScrollLock и NumLock
            
            break;
            
        case 0:
            controlLED(0x00);    //выключить индикаторы
            
            break;
            
        default:
            printf("Wrong parameter\n");
            return;
    }
}

//0x64 - порт ввода/вывода состояния контроллера клавиатуры
void controlLED(unsigned char mask)
{
    resend = 1;
    
    while(resend)    //пока требуются повторить передачу байта
    {
        while((inp(0x64) & 0x02) != 0x00);    //ожидать освобождения входного регистра контроллера клавиатуры
        
        outp(0x60, 0xED);    //отправить процессору клавиатуры команду управления светодиодами
    }
    
    resend = 1;
    
    while(resend)
    {
        while((inp(0x64) & 0x02) != 0x00);    //ожидать освобождения входного регистра контроллера клавиатуры
        
        outp(0x60, mask);
    }
}
