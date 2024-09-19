#include <msp430.h> 


/**
 * L1.c
 */
int main(void)
{
    int button_state = 0;

    WDTCTL = WDTPW | WDTHOLD;    // остановить сторжевой таймер

    // настройка порта для диода LED3 (P8.2)
    P8DIR |= BIT2;    // P8.2 (LED3) как выход
    P8OUT &= ~BIT2;    // выключить LED3

    // настройка порта для кнопки S1 (P1.7)
    P1DIR &= ~BIT7;    // P1.7 (S1) как вход
    P1REN |= BIT7;    // включить подтягивающий резистор
    P1OUT |= BIT7;    // подтягивание на P1.7

    while(1)
    {
        if(P1IN & BIT7)    // если кнопка нажата
        {
            __delay_cycles(100000);    // задержка импульса для предотвращения дребезга кнопки

            if(P1IN & BIT7)
            {
                if(button_state == 0)
                {
                    button_state = 1;

                    P8OUT |= BIT2;    // включить LED3

                    __delay_cycles(100000);

                    P8OUT &= ~BIT2;    // выключить LED3
                }
            }
        }

        if(!(P1IN & BIT7))    // если кнопка отпущена
        {
            __delay_cycles(100000);

            if(!(P1IN & BIT7))
            {
                if(button_state == 1)
                {
                    P8OUT |= BIT2;    // включить LED3

                    __delay_cycles(100000);    // задержка импульса

                    P8OUT &= ~BIT2;    // выключить LED3

                    button_state = 0;
                }
            }
        }
    }
}
