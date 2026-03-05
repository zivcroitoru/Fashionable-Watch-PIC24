/*
 * File:   main.c
 * Author: Amit Resh
 *
 * Created on May 21, 2022
 * Fixed Jan 2023
 */

#include <stdlib.h>
#include <stdio.h>

#include "System/system.h"
#include "System/delay.h"
#include "oledDriver/oledC.h"
#include "oledDriver/oledC_colors.h"
#include "oledDriver/oledC_shapes.h"

#include "Accel_i2c.h"


void errorStop(char *msg)
{
    oledC_DrawString(0, 20, 2, 2, msg, OLEDC_COLOR_DARKRED);

    for (;;)
        ;
}

/*
                         Main application
 */
int main(void)
{
    int rc;
    SYSTEM_Initialize();

     oledC_setBackground(OLEDC_COLOR_SKYBLUE);
     oledC_clearScreen();

     //Open i2C communications


    //Get Slave ID from slave address 0x1D (ID should be 0xE5)


    if (rc == OK) {
        //Display ADXL345 on top row of OLED
    }
    else
        errorStop("I2C Error");

    //Put accelerometer Power-Mode to "Measure"
    
       
    for (;;) {
        
        //Read X, Y, Z Accelerometer values
        
        //  === Display Axes Acceleration (OLEDC_COLOR_BLACK)  ====================

        DELAY_milliseconds(500);
        
        //  === Erase Axes Acceleration (OLEDC_COLOR_SKYBLUE)  ====================
        
    }
}
/**
 End of File
*/