#include "h/alarm.h"
#include "h/watch.h"
#include "../oledDriver/oledC.h"
#include "../oledDriver/oledC_shapes.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// shared state from other modules
extern volatile WatchTime now; // current time updated every sec
extern WatchTime alarmTime; // user's defined alarm time
extern bool alarmEnabled; // is alarm enabled by user?
extern volatile AppState myState; //global app state
extern volatile bool g_force_redraw; // flag to refresh display

// internal alarm state
static bool alarm_ringing = false; // is alarm currently active
static uint8_t alarm_seconds_left = 0; // how long alarm should keep ringing
static bool alarm_inverse_phase = false; // for blinking effect

static void alarm_set_inverse(bool inverse) // toggle OLED blinking inverse mode
{
    oledC_sendCommand(
        inverse ? OLEDC_CMD_SET_DISPLAY_MODE_INVERSE
                : OLEDC_CMD_SET_DISPLAY_MODE_ON,
        0,
        0
    );
}
// draw the alarm screen (called while ringing)
static void draw_alarm_screen(void)
{
    char buf[16];

    oledC_setBackground(0x0000);
    oledC_clearScreen(); // clear prev content
    
    // draw title + time
    sprintf(buf, "%02u:%02u", alarmTime.hour, alarmTime.min);

    oledC_DrawString(18, 28, 2, 2, "ALARM", 0xFFFF);
    oledC_DrawString(24, 56, 2, 2, buf, 0xFFFF);
}
// check if alarm is ringing now
bool alarm_is_ringing(void)
{
    return alarm_ringing;
}
// stop the alarm (manually or timeout)
void alarm_stop(void)
{
    alarm_ringing = false;
    alarm_seconds_left = 0;
    alarm_inverse_phase = false;

    alarm_set_inverse(false);

    oledC_setBackground(0x0000);
    oledC_clearScreen();
    
    // return to clock state
    if (myState == STATE_ALARM_RINGING) {
        myState = STATE_CLOCK;
    }

    g_force_redraw = true;  // force UI redraw after stopping
}
// called once per sec from main loop
void alarm_update_1s(void)
{
    // -- CASE 1 : alarm isn't ringing --
    
    if (!alarm_ringing)
    {
        // check if it's time to trigger alarm
        if (alarmEnabled &&
            now.hour == alarmTime.hour &&
            now.min  == alarmTime.min &&
            now.sec  == 0)
        {
            alarm_ringing = true;
            alarm_seconds_left = 20;    // alarm duration
            alarm_inverse_phase = true; // start with inverted screen
            myState = STATE_ALARM_RINGING;

            alarm_set_inverse(alarm_inverse_phase);
            g_force_redraw = true;
        }

        return;
    }
    // -- CASE 2 : alarm IS RINGING --
    
    // toggle screen inversion effect
    alarm_inverse_phase = !alarm_inverse_phase;
    alarm_set_inverse(alarm_inverse_phase);
    // decrease remaining alarm time
    if (alarm_seconds_left > 0) {
        alarm_seconds_left--;
    }
    // stop alarm when timer runs out
    if (alarm_seconds_left == 0) {
        alarm_stop();
        return;
    }

    g_force_redraw = true; // keep updating the alarm screen
}
// draw alarm UI only if active (called from display manager)
void alarm_draw_if_active(void)
{
    if (!alarm_ringing) {
        return;
    }

    draw_alarm_screen();
}