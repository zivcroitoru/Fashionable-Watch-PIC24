#include "h/watch.h"
#include "h/menu.h"
#include <xc.h>
#include "./../Accel_i2c.h"

#define LONG_PRESS_MS 2000

extern volatile AppState myState;
extern volatile uint32_t ms_ticks;

typedef struct
{
    bool is_down;
    uint32_t start_time;
} BtnStatus;

static BtnStatus s1_status = {false, 0};
static BtnStatus s2_status = {false, 0};

void check_inputs(void)
{
    bool s1_active = (PORTAbits.RA11 == 0); // S1
    bool s2_active = (PORTAbits.RA12 == 0); // S2

    // ---------------- S1 ----------------
    if (s1_active && !s1_status.is_down)
    {
        s1_status.is_down = true;
        s1_status.start_time = ms_ticks;
        LATAbits.LATA0 = 1;
    }
    else if (!s1_active && s1_status.is_down)
    {
        uint32_t duration = ms_ticks - s1_status.start_time;
        s1_status.is_down = false;
        LATAbits.LATA0 = 0;

        if (duration >= LONG_PRESS_MS)
        {
            if (myState == STATE_CLOCK)
            {
                menu_enter();
            }
            else if (myState == STATE_MENU)
            {
                menu_exit();
            }
        }
        else
        {
            if (myState == STATE_MENU)
            {
                menu_next_item();
            }
        }
    }

    // ---------------- S2 ----------------
    if (s2_active && !s2_status.is_down)
    {
        s2_status.is_down = true;
        s2_status.start_time = ms_ticks;
        LATAbits.LATA1 = 1;
    }
    else if (!s2_active && s2_status.is_down)
    {
        s2_status.is_down = false;
        LATAbits.LATA1 = 0;

        if (myState == STATE_MENU)
        {
            menu_select_current();
        }
    }
}

void check_gestures(void)
{
    unsigned char xl, xh, yl, yh, zl, zh;
    int16_t x, y, z;

    if (i2cReadSlaveRegister(0x3A, 0x32, &xl) != OK) return;
    if (i2cReadSlaveRegister(0x3A, 0x33, &xh) != OK) return;
    if (i2cReadSlaveRegister(0x3A, 0x34, &yl) != OK) return;
    if (i2cReadSlaveRegister(0x3A, 0x35, &yh) != OK) return;
    if (i2cReadSlaveRegister(0x3A, 0x36, &zl) != OK) return;
    if (i2cReadSlaveRegister(0x3A, 0x37, &zh) != OK) return;

    x = (int16_t)(((uint16_t)xh << 8) | xl);
    y = (int16_t)(((uint16_t)yh << 8) | yl);
    z = (int16_t)(((uint16_t)zh << 8) | zl);

    // Very simple debug test:
    // LED1 ON when motion is detected
    if ((x > 30 || x < -30) ||
        (y > 30 || y < -30) ||
        (z > 30 || z < -30))
    {
        LATAbits.LATA0 = 1;
    }
    else
    {
        LATAbits.LATA0 = 0;
    }
}