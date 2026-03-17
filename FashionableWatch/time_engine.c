#include "h/watch.h"
#include <xc.h>

// Global settings and state variables
volatile AppState myState = STATE_CLOCK;  // Current global mode
volatile FaceType myFace = FACE_DIGI;    // Visual style (digital/analog)
volatile TimeFormat myFormat = TIME_12H; // 12-hour or 24-hour preference

// The current real-time clock data
volatile WatchTime now = {0,0,12,1,1};

// Alarm settings
WatchTime alarmTime = {0,30,7,1,1};
bool alarmEnabled;

// System timing and redraw flags
volatile uint32_t ms_ticks = 0;      // Total milliseconds passed
volatile bool g_tick_1s = false;     // High every time a second passes
volatile bool g_force_redraw = true; // High when screen needs an update


// Returns maximum days in a month
uint8_t get_days_in_month(uint8_t month) {
    switch(month) {
        case 2: return 28; // February
        case 4: case 6: case 9: case 11: return 30; // April, June, Sept, Nov
        default: return 31; // All other months
    }
}

// Timer 1 Interrupt: Runs once every second to update the clock
void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void)
{
    _T1IF = 0; // Clear the interrupt flag

    ms_ticks += 1000;

    // Increment time and handle "rollover" (seconds -> minutes -> hours -> days)
    now.sec++;
    if (now.sec >= 60) {
        now.sec = 0;
        now.min++;

        if (now.min >= 60) {
            now.min = 0;
            now.hour++;

            if (now.hour >= 24) {
                now.hour = 0;   // Reset to midnight
                now.day++;      // Move to next calendar day

                // Check if we reached the end of the month
                if (now.day > get_days_in_month(now.month)) {
                    now.day = 1;
                    now.month++;

                    // Check if we reached the end of the year
                    if (now.month > 12)
                        now.month = 1;
                }
            }
        }
    }

    g_tick_1s = true; // Signal the main loop that a second has passed
}

// Converts internal 24h time to the user's preferred display format
uint8_t get_display_hour(uint8_t hour)
{
    if (myFormat == TIME_24H)
        return hour;

    // Standard 12-hour conversion (0 and 12 become 12)
    uint8_t h = hour % 12;
    return (h == 0) ? 12 : h;
}