#include "h/alarm.h"
#include "h/watch.h"
#include "../oledDriver/oledC.h"
#include "../oledDriver/oledC_shapes.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

extern volatile WatchTime now;
extern WatchTime alarmTime;
extern bool alarmEnabled;
extern volatile AppState myState;
extern volatile bool g_force_redraw;

static bool alarm_ringing = false;
static uint8_t alarm_seconds_left = 0;
static bool alarm_inverse_phase = false;

static void alarm_set_inverse(bool inverse)
{
    oledC_sendCommand(
        inverse ? OLEDC_CMD_SET_DISPLAY_MODE_INVERSE
                : OLEDC_CMD_SET_DISPLAY_MODE_ON,
        0,
        0
    );
}

static void draw_alarm_screen(void)
{
    char buf[16];

    oledC_setBackground(0x0000);
    oledC_clearScreen();

    sprintf(buf, "%02u:%02u", alarmTime.hour, alarmTime.min);

    oledC_DrawString(18, 28, 2, 2, "ALARM", 0xFFFF);
    oledC_DrawString(24, 56, 2, 2, buf, 0xFFFF);
}

bool alarm_is_ringing(void)
{
    return alarm_ringing;
}

void alarm_stop(void)
{
    alarm_ringing = false;
    alarm_seconds_left = 0;
    alarm_inverse_phase = false;

    alarm_set_inverse(false);

    oledC_setBackground(0x0000);
    oledC_clearScreen();

    if (myState == STATE_ALARM_RINGING) {
        myState = STATE_CLOCK;
    }

    g_force_redraw = true;
}

void alarm_update_1s(void)
{
    if (!alarm_ringing)
    {
        if (alarmEnabled &&
            now.hour == alarmTime.hour &&
            now.min  == alarmTime.min &&
            now.sec  == 0)
        {
            alarm_ringing = true;
            alarm_seconds_left = 20;
            alarm_inverse_phase = true;
            myState = STATE_ALARM_RINGING;

            alarm_set_inverse(alarm_inverse_phase);
            g_force_redraw = true;
        }

        return;
    }

    alarm_inverse_phase = !alarm_inverse_phase;
    alarm_set_inverse(alarm_inverse_phase);

    if (alarm_seconds_left > 0) {
        alarm_seconds_left--;
    }

    if (alarm_seconds_left == 0) {
        alarm_stop();
        return;
    }

    g_force_redraw = true;
}

void alarm_draw_if_active(void)
{
    if (!alarm_ringing) {
        return;
    }

    draw_alarm_screen();
}