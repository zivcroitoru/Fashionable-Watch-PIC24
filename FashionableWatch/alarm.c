#include "h/alarm.h"
#include "h/watch.h"
#include "../oledDriver/oledC.h"
#include "../oledDriver/oledC_shapes.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Shared state from other modules
extern volatile WatchTime now;         // current time updated every sec
extern WatchTime alarmTime;            // user's defined alarm time
extern bool alarmEnabled;              // is alarm enabled by user?
extern volatile AppState myState;      // global app state
extern volatile bool g_force_redraw;   // flag to refresh display

// Internal alarm state
static bool alarm_ringing = false;         // is alarm currently active
static uint8_t alarm_seconds_left = 0;     // how long alarm should keep ringing
static bool alarm_inverse_phase = false;   // for blinking effect

// Prevent retriggering many times during the same matching minute
static bool alarm_triggered_this_minute = false;

// Switch OLED between inverse and normal mode
static void alarm_set_inverse(bool inverse)
{
    oledC_sendCommand(
        inverse ? OLEDC_CMD_SET_DISPLAY_MODE_INVERSE
                : OLEDC_CMD_SET_DISPLAY_MODE_ON,
        0,
        0
    );
}

// Draw the alarm screen once when alarm starts
static void draw_alarm_screen(void)
{
    char buf[16];

    oledC_DrawRectangle(0, 0, 95, 95, 0x0000);

    sprintf(buf, "%02u:%02u", alarmTime.hour, alarmTime.min);

    oledC_DrawString(18, 28, 2, 2, (uint8_t*)"ALARM", 0xFFFF);
    oledC_DrawString(24, 56, 2, 2, (uint8_t*)buf, 0xFFFF);
}

// Check if alarm is currently ringing
bool alarm_is_ringing(void)
{
    return alarm_ringing;
}

// Stop alarm manually or after timeout
void alarm_stop(void)
{
    alarm_ringing = false;
    alarm_seconds_left = 0;
    alarm_inverse_phase = false;

    // ? Disable alarm after it finishes
    alarmEnabled = false;

    // Return display to normal mode
    alarm_set_inverse(false);

    // Clear screen once when leaving alarm mode
    oledC_DrawRectangle(0, 0, 95, 95, 0x0000);

    // Return to normal clock state
    if (myState == STATE_ALARM_RINGING) {
        myState = STATE_CLOCK;
    }

    g_force_redraw = true;
}

// Called once per second
void alarm_update_1s(void)
{
    // Reset minute trigger once time no longer matches
    if (now.hour != alarmTime.hour || now.min != alarmTime.min) {
        alarm_triggered_this_minute = false;
    }

    // If not ringing yet, check if it's time to start
    if (!alarm_ringing)
    {
        if (alarmEnabled &&
            now.hour == alarmTime.hour &&
            now.min  == alarmTime.min &&
            !alarm_triggered_this_minute)
        {
            alarm_ringing = true;
            alarm_seconds_left = 20;        // ring for 20 seconds
            alarm_inverse_phase = true;     // start inverted
            alarm_triggered_this_minute = true;
            myState = STATE_ALARM_RINGING;

            // Draw screen only once
            draw_alarm_screen();
            alarm_set_inverse(alarm_inverse_phase);
        }

        return;
    }

    // While ringing: only toggle invert state
    alarm_inverse_phase = !alarm_inverse_phase;
    alarm_set_inverse(alarm_inverse_phase);

    // Count down remaining time
    if (alarm_seconds_left > 0) {
        alarm_seconds_left--;
    }

    // Stop when time is over
    if (alarm_seconds_left == 0) {
        alarm_stop();
        return;
    }
}

// No redraw here, screen was already drawn once
void alarm_draw_if_active(void)
{
    if (!alarm_ringing) {
        return;
    }
}