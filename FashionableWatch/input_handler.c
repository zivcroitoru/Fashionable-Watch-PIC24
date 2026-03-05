#include "h/watch.h"
#include <xc.h>

#define LONG_PRESS_MS 2000 

typedef struct {
    bool is_down;
    uint32_t start_time;
    bool already_triggered;
} BtnStatus;

static BtnStatus s1_status = {false,0,false};
static BtnStatus s2_status = {false,0,false};

void check_inputs(void)
{
    bool s1_active = (PORTAbits.RA11 == 0);
    bool s2_active = (PORTAbits.RA12 == 0);

    if (s1_active && !s1_status.is_down)
    {
        s1_status.is_down = true;
        s1_status.start_time = ms_ticks;
        s1_status.already_triggered = false;

        LATAbits.LATA0 = 1;
    }

    else if (s1_active && s1_status.is_down && !s1_status.already_triggered)
    {
        if ((ms_ticks - s1_status.start_time) >= LONG_PRESS_MS)
        {
            s1_status.already_triggered = true;

            myState = STATE_MENU;
            g_force_redraw = true;
        }
    }

    else if (!s1_active && s1_status.is_down)
    {
        s1_status.is_down = false;
        LATAbits.LATA0 = 0;
    }

    if (s2_active && !s2_status.is_down)
    {
        s2_status.is_down = true;

        LATAbits.LATA1 = 1;

        if (myState == STATE_CLOCK)
        {
            myFace = (myFace == FACE_DIGI) ? FACE_ANALOG : FACE_DIGI;
            g_force_redraw = true;
        }
    }

    else if (!s2_active && s2_status.is_down)
    {
        s2_status.is_down = false;
        LATAbits.LATA1 = 0;
    }
}

void check_gestures(void)
{
}