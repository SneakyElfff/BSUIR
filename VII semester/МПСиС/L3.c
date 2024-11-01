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
    P8DIR |= BIT2;  // LED3 (P8.2) как вход
    P1DIR |= BIT1;  // LED4 (P1.1) как вход
    P1DIR |= BIT2;  // LED5 (P1.2) как вход
    P1DIR |= BIT3;  // LED6 (P1.3) как вход
    P1DIR |= BIT4;  // LED7 (P1.4) как вход
    P1DIR |= BIT5;  // LED8 (P1.5) как вход

    //P1OUT &= BIT1;
    P1OUT &= ~BIT3;  // светодиод LED6 выключен

    P7DIR |= BIT7;  // порт MCLK (P7.7)
    P7SEL |= BIT7;  // вывод на периферию

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

    UCSCTL3 = SELREF__REFOCLK;  // источник для FLL
    UCSCTL3 |= FLLREFDIV__1;  // делитель частоты FLL = 1

    UCSCTL4 = SELM__DCOCLK;  // выбор источника для MCLK

    UCSCTL1 = DCORSEL_2;  // выбор диапазона частот DCO (0,32 МГц — 7,38 МГц)

    // Fdcoclk = FLLrefclk * (FLLN + 1) = 2,68 МГц
    UCSCTL2 = FLLN6 + FLLN4 + FLLN0; // 0b0000000101000001 = 81
    UCSCTL2 |= FLLD__1;  // делитель FLL

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
    // выбор источника, режим, делитель на входе, очистка счетчика
    TA2CTL = TASSEL_1 + MC_1 + ID_1 + TACLR;  // MCLK, up

    TA2CCR0 = timerSpeed;  // сравнить с
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
    PMMCTL0_H = PMMPW_H;  // разблокировать регистр для записи

    // поля аналогичны SVSMHCTL, но по выходу
    SVSMLCTL = SVSLE | SVSLRVL0 * level | SVMLE | SVSMLRRL0 * level;  // управление SVS, SVM по выходу

    while (!(PMMIFG & SVSMLDLYIFG))  // достигнута задержка SVS, SVM
    ;

    SVSMLCTL = SVSLE | SVSLRVL0 * level;  // для проверки уровня Vcore

    PMMCTL0_L = PMMCOREV0 * level;

    SVSMLCTL = SVSLE | SVSLRVL0 * level | SVMLE | SVSMLRRL0 * level;

    while (!(PMMIFG & SVSMLDLYIFG))
    ;

    PMMCTL0_H = 0;
}

void increaseSupplyLevel(int level) {
    PMMCTL0_H = PMMPW_H;

    // SVSHE - разрешить SVS, SVSHRVL0 - Uвх для сброса, SVMHE - разрешить SVM, SVSMHRRL0 - Uвх для отклика
    SVSMHCTL = SVSHE | SVSHRVL0 * level | SVMHE | SVSMHRRL0 * level;  // изменить уровни SVM и SVS (Uвх > Vcore)
    SVSMLCTL = SVMLE | SVSMLRRL0 * level;

    while (!(PMMIFG & SVSMLDLYIFG))  // SVML стабилизировался
    ;

    PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);  // SVMLVLRIFG - достигнут Uвх, SVMLIFG - Uывх

    PMMCTL0_L = PMMCOREV0 * level;

    if ((PMMIFG & SVMLIFG))  // ожидание установки Vcore
    while (!(PMMIFG & SVMLVLRIFG))
      ;

    SVSMLCTL = SVSLE | SVSLRVL0 * level;
    PMMCTL0_H = 0;
}

void changeFrequency() {
    if (supplyVoltageLevel == 0) {
        supplyVoltageLevel = 1;
        increaseSupplyLevel(1);

        UCSCTL5 |= DIVM__1; // делитель MCLK на 1
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
    int temp = 0;

    P1IE = BIT7;  // включить прерывания для S1

    if (button == 0) {  // кнопка S1
        temp = (P1IN & BIT7);

        if (temp == 0) {
            P1OUT ^= BIT0;  // переключить LED1
            P8OUT ^= BIT2;  // переключить LED3

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
    else if (button == 1) {  // кнопка S2
        temp = (P2IN & BIT2);

        if (temp == 0) {
            P1OUT ^= BIT1;  // переключить LED4
            P1OUT ^= BIT2;  // переключить LED5

            changeFrequency();
        }

        P2IE = BIT2;
        P2IFG = 0;
    }

    TA2CCTL0 &= ~CCIE;
    TA2CTL = 0;
}
