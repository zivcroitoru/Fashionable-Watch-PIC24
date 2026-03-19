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

// Pot -> menu handlers
void menu_set_time_update_from_pot(void);
void menu_set_date_update_from_pot(void);
void menu_alarm_update_from_pot(void);

// I2C driver funcs
void i2c1_driver_open(void);
void i2c1_driver_close(void);

uint8_t analogTheme = 0;
volatile uint16_t g_pot_value = 0; // Global pot value used by menu screens

#define ADXL_ADDR 0x3A  

// ============================================================================
// LOCAL HELPERS
// ============================================================================

/**
 * Hard reset for the I2C bus. 
 */
static void i2c_force_reset(void)
{
    i2c1_driver_close();
    I2C1STAT = 0x0000;      // Clear any pending error flags
    DELAY_milliseconds(100);
    i2c1_driver_open();
    DELAY_milliseconds(100);
}

/**
 * Visual feedback for hardware failure.
 * Turns off LED A1 to signal a "halt" state.
 */
static void show_init_error(const char* msg)
{
    oledC_DrawRectangle(0, 0, 95, 95, 0x0000);
    oledC_DrawString(2, 5, 1, 1, (uint8_t*)msg, 0xFFFF);
    LATAbits.LATA1 = 0; 
}

/**
 * Standard ADXL345 startup sequence.
 */
static uint8_t accel_init(void)
{
    unsigned char devid = 0;
    I2Cerror rc = OK;
    uint8_t tries;

    i2c_force_reset();
    DELAY_milliseconds(200);

    // Verify we are actually talking to an ADXL345 (Expect 0xE5)
    for (tries = 0; tries < 3; tries++)
    {
        rc = i2cReadSlaveRegister(ADXL_ADDR, 0x00, &devid);
        if (rc == OK && devid == 0xE5)
            break;

        DELAY_milliseconds(50);
    }

    if (rc != OK)   return 10; // Connection failed
    if (devid != 0xE5) return 11; // Wrong device ID

    // Sensor must be in Standby (0x00) before changing certain data formats
    for (tries = 0; tries < 3; tries++)
    {
        rc = i2cWriteSlave(ADXL_ADDR, 0x2D, 0x00);
        if (rc == OK) break;
        DELAY_milliseconds(20);
    }
    if (rc != OK) return 14;

    DELAY_milliseconds(20);

    // Set data format to default (2g range)
    for (tries = 0; tries < 3; tries++)
    {
        rc = i2cWriteSlave(ADXL_ADDR, 0x31, 0x00);
        if (rc == OK) break;
        DELAY_milliseconds(20);
    }
    if (rc != OK) return 12;

    DELAY_milliseconds(20);

    // Switch to Measurement mode to start getting data
    for (tries = 0; tries < 3; tries++)
    {
        rc = i2cWriteSlave(ADXL_ADDR, 0x2D, 0x08);
        if (rc == OK) break;
        DELAY_milliseconds(20);
    }
    if (rc != OK) return 13;

    DELAY_milliseconds(100); // Let the sensor stabilize

    return 1;
}

// ============================================================================
// POTENTIOMETER UPDATE
// ============================================================================

/**
 * Reads the analog Potentiometer and updates the UI if the change is significant.
 */
void pot_update(void)
{
    static uint16_t prevVal = 0;
    uint16_t currVal;

    // Trigger manual ADC conversion
    AD1CON1 |= (1 << 1);       // SAMP = 1 (Start sampling)
    DELAY_milliseconds(5);     // Charge time for the internal cap
    AD1CON1 &= ~(1 << 1);      // SAMP = 0 (Start conversion)

    while ((AD1CON1 & 1) == 0) // Wait for DONE bit
    {
        ;
    }

    currVal = ADC1BUF0;

    // Deadzone/Hysteresis check: only update if value moved more than +/- 5
    // Prevents "jittering" on the screen due to electrical noise
    if ((currVal > (prevVal + 5)) || (currVal + 5 < prevVal))
    {
        g_pot_value = currVal;
        prevVal = currVal;

        // Route the pot value to the correct submenu
        if (myState == STATE_MENU)
        {
            switch (menu_get_current_page())
            {
                case MENU_PAGE_SET_TIME:
                    menu_set_time_update_from_pot();
                    break;
                case MENU_PAGE_SET_DATE:
                    menu_set_date_update_from_pot();
                    break;
                case MENU_PAGE_ALARM:
                    menu_alarm_update_from_pot();
                    break;
                default:
                    break;
            }
        }
    }
}

// ============================================================================
// SYSTEM INIT
// ============================================================================

void init_all(void)
{
    uint8_t st;

    __builtin_disable_interrupts();

    SYSTEM_Initialize();
    DELAY_milliseconds(300);

    // Accel is the most likely component to fail; check it first
    st = accel_init();
    if (st != 1)
    {
        DELAY_milliseconds(150); // Small cooldown before one last retry
        st = accel_init();
    }

    oledC_setup();

    // Catch hardware failure early and halt before enabling interrupts
    if (st != 1)
    {
        if (st == 10)       show_init_error("READ FAIL");
        else if (st == 11) show_init_error("BAD DEVID");
        else if (st == 12) show_init_error("WR31 FAIL");
        else if (st == 13) show_init_error("MEAS FAIL");
        else if (st == 14) show_init_error("STBY FAIL");
        else               show_init_error("ACCEL FAIL");

        __builtin_enable_interrupts();
        return;
    }

    // GPIO Setup
    TRISAbits.TRISA11 = 1; // Button 1
    TRISAbits.TRISA12 = 1; // Button 2

    TRISAbits.TRISA0 = 0;  // LED 1
    ANSELAbits.ANSA0 = 0;

    TRISAbits.TRISA1 = 0;  // LED 2
    ANSELAbits.ANSA1 = 0;

    LATAbits.LATA0 = 0;
    LATAbits.LATA1 = 0;

    // Timer1: 1-second heartbeat (assuming Fcy is roughly 16MHz)
    T1CON = 0x8030;
    PR1 = 15625;
    _T1IF = 0;
    _T1IE = 1;

    // ADC setup for Potentiometer on RB12 (AN8)
    TRISBbits.TRISB12 = 1;
    ANSELBbits.ANSB12 = 1;

    AD1CON1 = 0;
    AD1CON1 |= (1 << 15);     // Turn ADC ON
    AD1CON1 &= ~(1 << 2);     // Manual conversion trigger

    AD1CON2 = 0;              // Vref is AVDD/AVSS

    AD1CON3 = 0;
    AD1CON3 |= 0x00FF;        // Maximize Sample Time
    AD1CON3 |= (0x10 << 8);

    AD1CHS = 8;               // Set Channel to AN8

    // I2C SCL/SDA pins
    TRISBbits.TRISB8 = 1;
    TRISBbits.TRISB9 = 1;

    oledC_DrawRectangle(0, 0, 95, 95, 0x0000);

    __builtin_enable_interrupts();
}

// ============================================================================
// ACCEL READ
// ============================================================================

/**
 * Pulls X, Y, and Z raw data from the sensor.
 * Interrupts are disabled to prevent timing issues during I2C transactions.
 */
static uint8_t accel_read_xyz(int16_t* x, int16_t* y, int16_t* z)
{
    unsigned char x0, x1, y0, y1, z0, z1;

    __builtin_disable_interrupts();

    // Data registers start at 0x32 (X, Y, Z in order, 2 bytes each, Little Endian)
    if (i2cReadSlaveRegister(ADXL_ADDR, 0x32, &x0) != OK) goto fail;
    DELAY_microseconds(8); // Tiny gap for the bus to breathe

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

    // Combine High/Low bytes into signed 16-bit integers
    *x = (int16_t)((x1 << 8) | x0);
    *y = (int16_t)((y1 << 8) | y0);
    *z = (int16_t)((z1 << 8) | z0);

    return 1;

fail:
    __builtin_enable_interrupts();
    i2c_force_reset();
    accel_init();
    return 0;
}

// ============================================================================
// LOGIC CHECKS
// ============================================================================

/**
 * Checks if the watch is facing the floor (Z-axis negative).
 */
uint8_t accel_is_face_down(void)
{
    int16_t x, y, z;

    if (!accel_read_xyz(&x, &y, &z))
        return 0;

    return (z < -100) ? 1 : 0;
}

/**
 * Prints raw sensor data to the OLED for calibration and testing.
 */
void accel_debug_display(void)
{
    int16_t x, y, z;
    char buf[24];

    oledC_DrawRectangle(0, 0, 95, 95, 0x0000);

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

    // Orientation helper text
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