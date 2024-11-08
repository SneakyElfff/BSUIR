#include <msp430.h> 


/**
 * L4.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;  // остановить сторожевой таймер

    setupPins();
    setupADC();

    _enable_interrupt();

    while (1)
    {
        while (ADC12CTL0 & ADC12BUSY);  // ожидание освобождения АЦП
        // ADC12ENC - разрешить измерения, ADC12SC - запустить выборку и преобразования
        ADC12CTL0 |= ADC12ENC | ADC12SC;

        //__bis_SR_register(LPM0_bits + GIE); // LPM0, ADC12_ISR will force exit
    }
}

void setupPins()
{
//    P8DIR |= BIT0;  // установить P8.0 как выход
//    P8SEL &= ~BIT0;  // настроить P8.0 как цифровой ввод/вывод
//    P8OUT |= BIT0;  // установить высокий уровень на P8.0

    P1DIR |= BIT0;  // LED1 (P1.0) как вход
    P1OUT &= ~BIT0;  // светодиод LED1 выключен

    P8DIR |= BIT1;  // LED2 (P8.1) как вход
    P8OUT &= ~BIT1;  // светодиод LED2 выключен
}

void setupADC()
{
    // ADC12SHT0_2 - длительность выборки = 16, ADC12ON - включить АЦП, ADC12MSC - режим множественных выборок, ADC12REFON - включить опорный генератор
    ADC12CTL0 = ADC12SHT0_2 | ADC12ON | ADC12MSC | ADC12REFON | ADC12REF2_5V;
    // ADC12SHP - импульсный режим, ADC12CONSEQ_1 - последовательный режим
    ADC12CTL1 |= ADC12SHP | ADC12CONSEQ_1;
    ADC12CTL2 &= ~ADC12SR;  // частота выборки - 200К
    ADC12CTL2 |= ADC12RES_2;  // точность - 12 бит

    ADC12IE |= ADC12IE1;  // включить прерывания для регистра ADC12MEM1

    // входной канал 10 - температурный датчик
    ADC12MCTL0 |= ADC12INCH_10 | SELREF_0;  // пара опорных напряжений: VR+ = AVCC and VR- = AVSS
    ADC12MCTL1 |= ADC12EOS;

    // входной канал 8 - температурный датчик
    ADC12MCTL8 |= ADC12INCH_8;
    ADC12MCTL8 |= ADC12EOS;
}

#pragma vector=ADC12_VECTOR
__interrupt void adc_interrupt()
{

    int val_REF = ADC12MEM8;
    int val_TERM = ADC12MEM0;

    if (val_REF > val_TERM)
    {
        P1OUT |= BIT0;  // светодиод LED1 включен
        P8OUT &= ~BIT1;  // светодиод LED2 выключен
    }
    else
    {
        P1OUT &= ~BIT0;  // светодиод LED1 выключен
        P8OUT |= BIT1;  // светодиод LED2 включен
    }

    ADC12IFG &= ~ADC12IFG0;  // сбросить флаг прерывания
}
