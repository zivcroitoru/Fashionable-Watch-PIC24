#include "h/alarm.h"
#include "h/watch.h"
#include "../oledDriver/oledC.h"
#include <stdbool.h>
#include <stdint.h>

extern volatile WatchTime now;
extern WatchTime alarmTime;
extern bool alarmEnabled;
extern volatile AppState myState;
extern volatile bool g_force_redraw;

#define ALARM_DURATION_SEC 20

static bool alarm_ringing = false;
static uint8_t alarm_seconds_left = 0;
static bool alarm_inverse_phase = false;
static bool alarm_triggered_this_minute = false;

static void alarm_set_inverse(bool inverse)
{
    oledC_sendCommand(
        inverse ? OLEDC_CMD_SET_DISPLAY_MODE_INVERSE
                : OLEDC_CMD_SET_DISPLAY_MODE_ON,
        0,
        0
    );
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
    alarmEnabled = false;   // one-time alarm
    alarm_set_inverse(false);

    if (myState == STATE_ALARM_RINGING) {
        myState = STATE_CLOCK;
    }

    g_force_redraw = true;
}

void alarm_update_1s(void)
{
    bool time_matches =
        (now.hour == alarmTime.hour) &&
        (now.min  == alarmTime.min);

    if (!time_matches) {
        alarm_triggered_this_minute = false;
    }

    if (!alarm_ringing)
    {
        if (alarmEnabled &&
            time_matches &&
            !alarm_triggered_this_minute)
        {
            alarm_ringing = true;
            alarm_seconds_left = ALARM_DURATION_SEC;
            alarm_inverse_phase = true;
            alarm_triggered_this_minute = true;
            myState = STATE_ALARM_RINGING;

            alarm_set_inverse(true);
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
    }
}