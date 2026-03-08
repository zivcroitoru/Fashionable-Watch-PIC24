#include "h/watch.h"
#include "h/menu.h"
#include "../System/delay.h"
#include <xc.h>
#include <stdint.h>

void pot_update(void)
{
    static uint16_t prevVal = 0;
    uint16_t currVal;

    AD1CON1 |= (1 << 1);    // start sampling
    DELAY_milliseconds(5);
    AD1CON1 &= ~(1 << 1);   // start conversion

    while ((AD1CON1 & 1) == 0)
    {
        ;
    }

    currVal = ADC1BUF0;

    if (currVal > prevVal + 5 || currVal < prevVal - 5)
    {
        g_pot_value = currVal;
        menu_set_time_update_from_pot();
        prevVal = currVal;
    }
}