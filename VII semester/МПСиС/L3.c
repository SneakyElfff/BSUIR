#include <msp430.h>


/**
 * L3.c
 */
int level = 0, delay, frequency, supplyVoltageLevel, button;
int powerSaving = 0;
int check = 0;
int timerSpeed = 1000;
void decreaseSupplyLevel(int level);

int main(void) {
    long int counter = 0;

    WDTCTL = WDTPW | WDTHOLD;  // Остановить сторожевой таймер

    P1DIR |= BIT0;  // LED1 (P1.0) как вход
    P1DIR |= BIT1;  // LED4 (P1.1) как вход
    P1DIR |= BIT2;  // LED5 (P1.2) как вход
    P1DIR |= BIT3;  // LED6 (P1.3) как вход
    P1DIR |= BIT4;  // LED7 (P1.4) как вход
    P1DIR |= BIT5;  // LED8 (P1.5) как вход

    P8DIR |= BIT2; // LED3 (P8.2) как выход

    //P1OUT &= BIT1;
    P1OUT &= ~BIT3;  // светодиод LED6 выключен

    P7DIR |= BIT7;  // порт MCLK
    P7SEL |= BIT7;  // вывод как перифирийный

    P1REN |= BIT7;  // включить подтяжку кнопки S1 (P1.7)
    P1OUT |= BIT7;  // подтянуть кнопку S1 к Vcc

    P2REN |= BIT2;  // включить подтяжку кнопки S2 (P2.2)
    P2OUT |= BIT2;  // подтянуть кнопку S2 к Vcc

    P1IE |= BIT7;  // включить прерывания для S1
    P2IE |= BIT2;  // для S2

    P1IFG &= ~BIT7;  // очистить флаг прерывания для S1
    P2IFG &= ~BIT2;  // для S2

    P1IES |= BIT7;  // прерывание по спадающему фронту для S1
    P2IES |= BIT2;  // для S1

    P1OUT |= BIT2;  // включить LED5 (низкое напряжение)
    P1OUT |= BIT0;  // включить LED1 (активный режим)
    P1OUT &= ~BIT1;  // выключить LED4 (высокое напряжение)

    P1IFG = 0;
    P8OUT &= ~BIT2;  // светодиод LED3 выключен

    __bis_SR_register(GIE);  // глобальное разрешение прерываний

    UCSCTL3 = SELREF__REFOCLK;
    UCSCTL3 |= FLLREFDIV__1;
    UCSCTL4 = SELM__DCOCLK;
    UCSCTL1 = DCORSEL_2;

    UCSCTL2 = FLLN6 + FLLN4 + FLLN0;
    UCSCTL2 |= FLLD__1;

    while(1) {
        counter++;

        if (counter >= 100000) {
            counter = 0;
            P1OUT ^= BIT5;  // переключить LED8
        }
    }

    return 0;
}

void startTA2() {
    TA2CTL = TASSEL_1 + MC_1 + ID_1 + TACLR;
    TA2CCR0 = timerSpeed;
    TA2CCTL0 |= CCIE;
}

// прерывание от кнопки S1 для переключения тактовой частоты и напряжения
#pragma vector=PORT1_VECTOR
__interrupt void P1INT() {
    P1IE = 0;
    P1IFG = 0;

    button = 0;

    startTA2();
}

// прерывание от кнопки S2 для переключения режима пониженного энергопотребления
#pragma vector=PORT2_VECTOR
__interrupt void P2INT() {
    P2IE = 0;
    P2IFG = 0;

    button = 1;

    startTA2();
}

void decreaseSupplyLevel(int level) {
    PMMCTL0_H = PMMPW_H;
    SVSMLCTL = SVSLE | SVSLRVL0 * level | SVMLE | SVSMLRRL0 * level;

    while (!(PMMIFG & SVSMLDLYIFG))
    ;
    SVSMLCTL = SVSLE | SVSLRVL0 * level;

    PMMCTL0_L = PMMCOREV0 * level;
    SVSMLCTL = SVSLE | SVSLRVL0 * level | SVMLE | SVSMLRRL0 * level;

    while (!(PMMIFG & SVSMLDLYIFG))
    ;
    PMMCTL0_H = 0;
}

void increaseSupplyLevel(int level) {
    PMMCTL0_H = PMMPW_H;
    SVSMHCTL = SVSHE | SVSHRVL0 * level | SVMHE | SVSMHRRL0 * level;
    SVSMLCTL = SVMLE | SVSMLRRL0 * level;

    while (!(PMMIFG & SVSMLDLYIFG))
    ;

    PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);
    PMMCTL0_L = PMMCOREV0 * level;

    if ((PMMIFG & SVMLIFG))
    while (!(PMMIFG & SVMLVLRIFG))
      ;

    SVSMLCTL = SVSLE | SVSLRVL0 * level;
    PMMCTL0_H = 0;
}

void changeFrequency() {
    if (supplyVoltageLevel == 0) {
        supplyVoltageLevel = 1;
        increaseSupplyLevel(1);

        UCSCTL5 |= DIVM__1;
        UCSCTL5 &= ~DIVM__8;
        timerSpeed = 1000;
    }
    else if (supplyVoltageLevel == 1) {
        supplyVoltageLevel = 0;
        decreaseSupplyLevel(0);

        UCSCTL5 &= ~DIVM__1;
        UCSCTL5 |= DIVM__8;
        timerSpeed = 250;
    }
}

#pragma vector = TIMER2_A0_VECTOR
__interrupt void TIMER222() {
    int a = 90;

    P1IE = BIT7;  // включить прерывания для S1

    if (button == 0) {
        a = (P1IN & BIT7);

        if (a == 0) {
            P1OUT ^= BIT0;
            P8OUT ^= BIT2;

            if (powerSaving == 0) {
                powerSaving = 1;

                __bis_SR_register_on_exit(LPM0_bits);

            }
            else if (powerSaving == 1) {
                powerSaving = 0;

                __bic_SR_register_on_exit(LPM0_bits);
            }
        }

    P1IFG = 0;
    }
    else if (button == 1) {
        a = (P2IN & BIT2);

        if (a == 0) {
            P1OUT ^= BIT1;
            P1OUT ^= BIT2;

            changeFrequency();
        }

        P2IE = BIT2;
        P2IFG = 0;
    }

    TA2CCTL0 &= ~CCIE;
    TA2CTL = 0;
}
