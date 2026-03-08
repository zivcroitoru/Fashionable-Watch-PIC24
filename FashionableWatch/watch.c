#include "h/watch.h"
#include "h/menu.h"
#include "../System/system.h"
#include "../System/delay.h"
#include "../Accel_i2c.h"
#include "../oledDriver/oledC.h"
#include "../oledDriver/oledC_shapes.h"
#include "../oledDriver/oledC_colors.h"

#include <stdint.h>
#include <stdio.h>
#include <xc.h>

uint8_t analogTheme = 0;
volatile uint16_t g_pot_value = 0;

#define ADXL_ADDR 0x3A   // ADXL345 8-bit write address (ALT high)

// ============================================================================
// ERROR DISPLAY
// ============================================================================

static void show_init_error(const char* msg)
{
    oledC_DrawRectangle(0,0,95,95,0x0000);
    oledC_DrawString(2,5,1,1,(uint8_t*)msg,0xFFFF);
    LATAbits.LATA1 = 0;
}

// ============================================================================
// POTENTIOMETER UPDATE
// ============================================================================

void pot_update(void)
{
    static uint16_t prevVal = 0;
    uint16_t currVal;

    AD1CON1 |= (1 << 1);
    DELAY_milliseconds(5);
    AD1CON1 &= ~(1 << 1);

    while ((AD1CON1 & 1) == 0);

    currVal = ADC1BUF0;

    if (currVal > prevVal + 5 || currVal < prevVal - 5)
    {
        g_pot_value = currVal;
        menu_set_time_update_from_pot();
        prevVal = currVal;
    }
}

// ============================================================================
// SYSTEM INIT
// ============================================================================

void init_all(void)
{
    unsigned char devid = 0;

    __builtin_disable_interrupts();

    SYSTEM_Initialize();
    oledC_setup();

    // ------------------------------------------------
    // Buttons
    // ------------------------------------------------

    TRISAbits.TRISA11 = 1;
    TRISAbits.TRISA12 = 1;

    // ------------------------------------------------
    // LEDs
    // ------------------------------------------------

    TRISAbits.TRISA0 = 0;
    ANSELAbits.ANSA0 = 0;

    TRISAbits.TRISA1 = 0;
    ANSELAbits.ANSA1 = 0;

    LATAbits.LATA0 = 0;
    LATAbits.LATA1 = 0;

    // ------------------------------------------------
    // TIMER 1
    // ------------------------------------------------

    T1CON = 0x8030;
    PR1 = 15625;

    _T1IF = 0;
    _T1IE = 1;

    // ------------------------------------------------
    // ADC (Potentiometer RB12)
    // ------------------------------------------------

    TRISBbits.TRISB12 = 1;
    ANSELBbits.ANSB12 = 1;

    AD1CON1 = 0;
    AD1CON1 |= (1 << 15);
    AD1CON1 &= ~(1 << 2);

    AD1CON2 = 0;

    AD1CON3 = 0;
    AD1CON3 |= 0xFF;
    AD1CON3 |= (0x10 << 8);

    AD1CHS = 8;

    // ------------------------------------------------
    // I2C1 PINS  (RB8=SCL RB9=SDA)
    // ------------------------------------------------

    TRISBbits.TRISB8 = 1;
    TRISBbits.TRISB9 = 1;

//    ANSELBbits.ANSB8 = 0;
//    ANSELBbits.ANSB9 = 0;

    // ------------------------------------------------
    // I2C OPEN
    // ------------------------------------------------

    DELAY_milliseconds(200);

    i2c1_driver_close();
    I2C1STAT = 0x0000;
    DELAY_milliseconds(100);
    i2c1_driver_open();
    DELAY_milliseconds(100);

    // ------------------------------------------------
    // ADXL345 DEVICE ID CHECK
    // ------------------------------------------------

I2Cerror rc;

rc = i2cReadSlaveRegister(ADXL_ADDR, 0x00, &devid);
if (rc != OK || devid != 0xE5)
{
    DELAY_milliseconds(100);
    rc = i2cReadSlaveRegister(ADXL_ADDR, 0x00, &devid);
    if (rc != OK || devid != 0xE5)
    {
        show_init_error("DEVID FAIL");
        __builtin_enable_interrupts();
        return;
    }
}

    if (devid != 0xE5)
    {
        show_init_error("BAD ID");
        __builtin_enable_interrupts();
        return;
    }


    // ------------------------------------------------
    // ADXL CONFIG
    // ------------------------------------------------

    if (i2cWriteSlave(ADXL_ADDR,0x2D,0x00) != OK)
    {
        show_init_error("WR 2D FAIL");
        __builtin_enable_interrupts();
        return;
    }

    DELAY_milliseconds(10);

if (i2cWriteSlave(ADXL_ADDR, 0x31, 0x00) != OK)
{
    show_init_error("WR 31 FAIL");
    __builtin_enable_interrupts();
    return;
}
DELAY_milliseconds(10);

if (i2cWriteSlave(ADXL_ADDR, 0x2D, 0x08) != OK)
{
    show_init_error("WR MEAS FAIL");
    __builtin_enable_interrupts();
    return;
}
DELAY_milliseconds(100);

    DELAY_milliseconds(50);

    oledC_DrawRectangle(0,0,95,95,0x0000);

    __builtin_enable_interrupts();
}

// ============================================================================
// ACCEL READ
// ============================================================================
static uint8_t accel_read_xyz(int16_t* x, int16_t* y, int16_t* z)
{
    unsigned char x0, x1, y0, y1, z0, z1;

    __builtin_disable_interrupts();

    if (i2cReadSlaveRegister(ADXL_ADDR, 0x32, &x0) != OK) goto fail;
    DELAY_microseconds(8);

    if (i2cReadSlaveRegister(ADXL_ADDR, 0x33, &x1) != OK) goto fail;
    DELAY_microseconds(8);

    if (i2cReadSlaveRegister(ADXL_ADDR, 0x34, &y0) != OK) goto fail;
    DELAY_microseconds(8);

    if (i2cReadSlaveRegister(ADXL_ADDR, 0x35, &y1) != OK) goto fail;
    DELAY_microseconds(8);

    if (i2cReadSlaveRegister(ADXL_ADDR, 0x36, &z0) != OK) goto fail;
    DELAY_microseconds(8);

    if (i2cReadSlaveRegister(ADXL_ADDR, 0x37, &z1) != OK) goto fail;
    DELAY_microseconds(8);

    __builtin_enable_interrupts();

    *x = (int16_t)((x1 << 8) | x0);
    *y = (int16_t)((y1 << 8) | y0);
    *z = (int16_t)((z1 << 8) | z0);

    return 1;

fail:
    __builtin_enable_interrupts();
    return 0;
}
// ============================================================================
// FACE DOWN CHECK
// ============================================================================

uint8_t accel_is_face_down(void)
{
    int16_t x, y, z;

    if (!accel_read_xyz(&x, &y, &z))
        return 0;

    return (z < -100) ? 1 : 0;
}
void accel_debug_display(void)
{
    int16_t x, y, z;
    char buf[24];

    oledC_DrawRectangle(0,0,95,95,0x0000);

    if (!accel_read_xyz(&x, &y, &z))
    {
        oledC_DrawString(2, 10, 1, 1, (uint8_t*)"ACCEL READ FAIL", 0xFFFF);
        DELAY_milliseconds(200);
        return;
    }

    sprintf(buf, "X:%d", x);
    oledC_DrawString(2, 10, 1, 1, (uint8_t*)buf, 0xFFFF);

    sprintf(buf, "Y:%d", y);
    oledC_DrawString(2, 25, 1, 1, (uint8_t*)buf, 0xFFFF);

    sprintf(buf, "Z:%d", z);
    oledC_DrawString(2, 40, 1, 1, (uint8_t*)buf, 0xFFFF);

    if (z < -100)
    {
        oledC_DrawString(2, 60, 1, 1, (uint8_t*)"FACE DOWN", OLEDC_COLOR_RED);
    }
    else if (z > 100)
    {
        oledC_DrawString(2, 60, 1, 1, (uint8_t*)"FACE UP", OLEDC_COLOR_GREEN);
    }
    else
    {
        oledC_DrawString(2, 60, 1, 1, (uint8_t*)"SIDE", OLEDC_COLOR_BLUE);
    }

    DELAY_milliseconds(200);
}