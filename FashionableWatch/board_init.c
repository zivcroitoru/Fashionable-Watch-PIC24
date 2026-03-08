#include "h/watch.h"
#include "../System/system.h"
#include "../System/delay.h"
#include "../oledDriver/oledC.h"
#include <xc.h>

void init_all(void)
{
    __builtin_disable_interrupts();

    SYSTEM_Initialize();
    oledC_setup();

    // ---------------- Buttons ----------------
    TRISAbits.TRISA11 = 1;
    TRISAbits.TRISA12 = 1;

    // ---------------- LEDs ----------------
    TRISAbits.TRISA0 = 0;
    ANSELAbits.ANSA0 = 0;

    TRISAbits.TRISA1 = 0;
    ANSELAbits.ANSA1 = 0;

    LATAbits.LATA0 = 0;
    LATAbits.LATA1 = 0;

    // ---------------- Timer1 ----------------
    T1CON = 0x8030;
    PR1   = 15625;

    _T1IF = 0;
    _T1IE = 1;

    // ---------------- ADC (Potentiometer RB12) ----------------
    TRISBbits.TRISB12 = 1;
    ANSELBbits.ANSB12 = 1;

    AD1CON1 = 0;
    AD1CON1 |= (1 << 15);   // ADC on
    AD1CON1 &= ~(1 << 2);   // integer format

    AD1CON2 = 0;

    AD1CON3 = 0;
    AD1CON3 |= 0xFF;
    AD1CON3 |= (0x10 << 8);

    AD1CHS = 8;

    oledC_DrawRectangle(0, 0, 95, 95, 0x0000);

    __builtin_enable_interrupts();
}