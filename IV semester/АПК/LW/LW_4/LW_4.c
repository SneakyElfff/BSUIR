//
//  LW_4.c
//  LW_4
//
//  Created by Нина Альхимович on 31.03.23.
/*  В программе должно быть реализовано меню, позволяющее выбрать тестируемый функционал (установка времени, считывание времени, задержка и т.д.).
 1. Написать программу, которая будет считывать и устанавливать время в часах реального времени. Считанное время должно выводиться на экран в удобочитаемой форме.
 Условия выполнения:
       1) Перед началом установки новых значений времени необходимо считывать и анализировать старший байт регистра состояния 1 на предмет доступности значений для чтения и записи.
       2) После проверки доступности регистров, необходимо отключить внутренний цикл обновления часов реального времени, воспользовавшись старшим битом регистра состояния 2. После окончания операции установки значений этот бит должен быть сброшен, для возобновления внутреннего цикла обновления часов реального времени.
       3) Новые значения для регистров часов реального времени должны вводится с клавиатуры в удобном для пользователя виде.
    2. Используя аппаратное прерывание часов реального времени и режим генерации периодических прерываний, реализовать функцию задержки с точностью в миллисекунды (с возможностью изменения частоты генерации).
 Условия выполнения:
       1) Задержка должна вводится с клавиатуры в миллисекундах в удобной для пользователя форме.
       2) После окончания периода задержки программа должна сообщить об этом в любой форме. При этом зависание компьютера не допускается.
       3) Предусмотреть возможность изменения частоты генерации периодических прерываний вводом с клавиатуры соответствующих значений.
       Используя аппаратное прерывания часов реального времени и режим будильника ЧРВ (4A использовать не желательно), реализовать функции программируемого будильника.
       1) Время будильника вводится с клавиатуры в удобной для пользователя форме.
       2) При срабатывания будильника программа должна сообщить об этом в любой форме. При этом зависание компьютера не допускается.
*/
#include <stdio.h>
#include <string.h>
#include <dos.h>

void getCurrentTime(void);    //функция считывания времени в ЧРВ
void setCurrentTime(void);    //функция установки времени в ЧРВ
void setDelay(void);    //функция задержки
void setFrequency(void);    //функция изменения частоты генерации периодических прерываний
void setAlarm(void);    //функция установки будильника

void wait(void);    //функция ожидания конца цикла корректировки
int BCDToDecimal(int);    //функция перевода числа из двоично-десятичного формата в десятичный
int DecimalToBCD(int);    //функция перевода числа из двоично-десятичного формата в десятичный

volatile unsigned long delay_counter;
void interrupt(*systemRTCDelay)(void);    //указатель на прерывание ЧРВ

void interrupt customRTCDelay(void)    //прототип функции обработки прерывания
{
    if(delay_counter > 0)
        delay_counter--;
    
    (*systemRTCDelay)();
}

volatile int hours_alarm, mins_alarm, secs_alarm, trigger = 0;
void interrupt(*systemRTCAlarm)(void);

void interrupt customRTCAlarm(void)
{
    int temph, tempm, temps;
    
    wait();
    outp(0x70, 0x04);
    temph = BCDToDecimal(inp(0x71));

    wait();
    outp(0x70, 0x02);
    tempm = BCDToDecimal(inp(0x71));
    
    wait();
    outp(0x70, 0x00);
    temps = BCDToDecimal(inp(0x71));
    
    if(tempm == mins_alarm && temph == hours_alarm && temps == secs_alarm)
        trigger = 1;
    
    (*systemRTCAlarm)();
}

//0x70 - порт индекса ЧРВ, 0x71 - порт данных ЧРВ
int main()
{
    while(1)
    {
        puts("\nChoose an option: 1-get current time;");
        puts("                  2-set current time;");
        puts("                  3-set delay;");
        puts("                  4-setAlarm;");
        puts("                  0-end");
        fflush(stdin);

        switch(getchar())
        {
            case '1':
                getCurrentTime();    //считывание времени в ЧРВ

                break;

            case '2':
                setCurrentTime();    //установка времени в ЧРВ

                break;

            case '3':
                setDelay();    //задержка
                
                break;
                
            case '4':
                setAlarm();    //программируемый будильник
                
                break;

            case '0':
                return 0;

            default:
                fprintf(stderr, "Wrong parameter\n");
                return -1;
        }
    }
    
    return 0;
}

void getCurrentTime()
{
    printf("Current time: ");
    wait();
    outp(0x70, 0x04);    //в порт 70h посылается номер требуемого байта памяти, текущий час
    printf("%d:", BCDToDecimal(inp(0x71)));    //чтение байта памяти через порт 71h

    wait();
    outp(0x70, 0x02);    //текущая минута
    printf("%d:", BCDToDecimal(inp(0x71)));

    wait();
    outp(0x70, 0x00);    //текущая секунда
    printf("%d\n", BCDToDecimal(inp(0x71)));
}

void setCurrentTime()
{
    int hours, mins, secs;
    
    wait();
    outp(0x70, 0x0B);    //0Bh - регистр состояния B (бит 7 - запрещено обновление часов)
    outp(0x71, inp(0x71) | (1 << 6));    //отключить цикл обновления ЧРВ, установив 7-й бит в 1

    printf("Enter exact time you want to set: ");
    scanf("%d:%d:%d", &hours, &mins, &secs);    //извлечение значений часов, минут и секунд

    outp(0x70, 0x04);
    outp(0x71, DecimalToBCD(hours));

    outp(0x70, 0x02);
    outp(0x71, DecimalToBCD(mins));

    outp(0x70, 0x00);
    outp(0x71, DecimalToBCD(secs));

    outp(0x70, 0x0B);
    outp(0x71, inp(0x71) & ~(1 << 6));    //включить цикл обновления ЧРВ, сбросив 7-й бит (~ - побитовое инвертирование)
}

void setDelay()
{
    printf("Enter the length of delay in milliseconds: ");
    scanf("%u", &delay_counter);
    
    _disable();    //запретить прерывания (= cli)
    
    systemRTCDelay = getvect(0x70);    //сохранить указатель на старый обработчик прерываний ЧРВ
    
    setvect(0x70, customRTCDelay);    //установить новый обработчик прерываний ЧРВ
    
    _enable();    //разрешить прерывания (= sti)
    
    printf("Delay started\n");
    
    //setFrequency();
    
    //0xA1 - регистр маски для второго контроллера прерываний PIC
    outp(0xA1, inp(0xA1) & ~1);    //~1 = 1111 111 0 - разрешить прерывания на линии ЧРВ (IRQ8)
    outp(0x70, 0x0B);    //регистр состояния B
    outp(0x71, inp(0x71) | (1 << 6));    //6 бит - в 1: вызов периодического прерывания (IRQ8)
    
    while(delay_counter > 0);
    
    printf("Delay is over\n");
    
    setvect(0x70, systemRTCDelay);    //восстановить старый обработчик прерываний
}

void setFrequency()
{
    unsigned char freq;
    
    printf("Enter the frequency (in Hz) you want to set for RTC: ");
    scanf("%hhu", &freq);
    
    freq = (unsigned char)(32768 / freq);

    outp(0x70, 0x0A);

    outp(0x71, (inp(0x71) & 0xF0) | freq);    //сохранить старшие 4 бита регистра А (0xF0 = 11110000), с помощью | меняются 4 младший бита
}

void setAlarm()
{
    printf("Enter the alarm time: ");
    scanf("%d:%d:%d", &hours_alarm, &mins_alarm, &secs_alarm);
    
    _disable();    //запретить прерывания (= cli)
    
    systemRTCAlarm = getvect(0x70);    //сохранить указатель на старый обработчик прерываний ЧРВ
    
    setvect(0x70, customRTCAlarm);    //установить новый обработчик прерываний ЧРВ
    
    _enable();    //разрешить прерывания (= sti)
    
    outp(0x70, 0x0B);    //регистр состояния B
    outp(0x71, inp(0x71) | (1 << 5));    //5 бит - в 0: вызов периодического прерывания (IRQ8)
    
    while(!trigger);
    
    printf("ALARM!\n");
    
    setvect(0x70, systemRTCAlarm);    //восстановить старый обработчик прерываний
}

void wait()
{
    do
    {
        outp(0x70, 0x0A);    //0Аh - регистр состояния A (бит 7 - заняты/не заняты часы
    } while(inp(0x71) & (1 << 6));    //проверка значения 7-го бита: пока он не сброшен, происходит корректировка частка КМОП-памяти (первые десять байтов)
}

int BCDToDecimal(int bcd)
{
    int decimal = 0, factor = 1;

    while(bcd != 0)
    {
        decimal += (bcd & 0x0F) * factor;    //bcd & 0x0F - младшие четыре бита
        bcd >>= 4;    //обеспечение обработки следующих четырех битов
        factor *= 10;    //учет каждой десятичной позиции
    }

    return decimal;
}

int DecimalToBCD(int decimal)
{
    int bcd = 0, multiplier = 1;
    
    while(decimal > 0)
    {
        bcd += (decimal % 10) * multiplier;    //обработка каждой цифры десятичного числа
        multiplier *= 16;    //обеспечение перехода к следующему разряду кода
        decimal /= 10;    //обеспечение перехода к следующей цифре
    }
    
    return bcd;
}
