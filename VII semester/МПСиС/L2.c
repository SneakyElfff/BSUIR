#include <msp430.h>


/**
 * L2.c
 */
volatile unsigned char blink = 0;

void delay(unsigned int cycles) {
    TA0CCR0 = cycles;            // установить значение для счетчика
    TA0CTL |= TACLR;             // сбросить таймер
    TA0CTL |= TASSEL_2 | MC_1;   // использовать SMCLK и режим Up

    // ждать, пока таймер не достигнет значения CCR0
    while (!(TA0CTL & TAIFG));   // ждать флаг прерывания таймера
    TA0CTL &= ~TAIFG;            // сбросить флаг прерывания
}

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;     // остановить сторжевой таймер

    // настройка порта для кнопки S1 (P1.7)
    P1DIR &= ~BIT7;               // P1.7 (S1) как вход
    P1REN |= BIT7;                // включить подтягивающий резистор
    P1OUT |= BIT7;                // подтягивание вверх на P1.7

    P2DIR &= ~BIT2;
    P2REN |= BIT2;
    P2OUT |= BIT2;

    // настройка порта для диода LED2 (P8.1)
    P8DIR |= BIT1;                // P8.1 (LED2) как выход
    P8OUT &= ~BIT1;               // выключить LED2

    // настройка прерываний для кнопки S1
    P1IE |= BIT7;                 // включить прерывания на P1.7 (S1)
    P1IES |= BIT7;                // по спадающему фронту
    P1IFG &= ~BIT7;               // очистить флаг прерывания

    // настройка прерываний для кнопки S2
    P2IE |= BIT2;
    P2IES |= BIT2;
    P2IFG &= ~BIT2;

    // конфигурация WDT как интервального
    WDTCTL = WDT_MDLY_32;         // WDT в интервале ~32 мс
    SFRIE1 |= WDTIE;              // включить прерывания от WDT

    __bis_SR_register(GIE);       // включить глобальные прерывания

    while (1) {
        __bis_SR_register(LPM0_bits); // переход в режим низкого энергопотребления
        __no_operation();
    }
}

#pragma vector=PORT1_VECTOR
__interrupt void PORT_1(void) {
    if (P1IN & BIT7){
        delay(10000);
        if (P1IN & BIT7){
            blink ^= 1;
            P1IFG &= ~BIT7;
        }
    }

}

#pragma vector=PORT2_VECTOR
__interrupt void PORT_2(void) {
    if (P2IN & BIT2){
        delay(10000);
        if (P2IN & BIT2){
            blink ^= 1;
            P2IFG &= ~BIT2;
        }
    }
}

#pragma vector=WDT_VECTOR
__interrupt void WDT_ISR(void) {
    if (blink) {                 // если режим мигания включен
        P8OUT &= ~ BIT1;           // переключить состояние светодиода LED2
    } else {
        P8OUT ^= BIT1;
    }
}
