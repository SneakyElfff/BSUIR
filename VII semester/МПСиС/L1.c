#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
    int button_state = 0;

    WDTCTL = WDTPW | WDTHOLD;    // остановить сторжевой таймер

    // настройка порта для кнопки S1 (P1.7)
    P1DIR &= ~BIT7;    // P1.7 (S1) как вход
    P1REN |= BIT7;    // подтягивающий резистор
    P1OUT |= BIT7;    // подтягивание к Vcc

    // настройка порта для диода LED3 (P8.2)
    P8DIR |= BIT2;    // P8.2 (S1) как выход
    P8OUT &= ~BIT2;    // выключить LED3

    while(1)
    {
        if(!(P1IN & BIT7))    // если кнопка нажата (P1.7 = 0)
        {
            if(button_state == 0)
            {
                P8OUT |= BIT2;    // включить LED3
                __delay_cycles(50000);    // задержка импульса
                P8OUT &= ~BIT2;    // выключить LED3

                button_state = 1;
            }
        }
        else    // если кнопка отпущена (P1.7 = 1)
        {
            if(button_state == 1)
            {
                P8OUT |= BIT2;    // включить LED3
                __delay_cycles(50000);    // задержка импульса
                P8OUT &= ~BIT2;    // выключить LED3

                button_state = 0;
            }
        }
    }
}
