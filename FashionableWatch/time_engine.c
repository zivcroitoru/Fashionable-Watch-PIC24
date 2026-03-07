#include "h/watch.h"
#include <xc.h>

volatile AppState myState = STATE_CLOCK;
volatile FaceType myFace = FACE_DIGI;
volatile TimeFormat myFormat = TIME_12H;

volatile WatchTime now = {0,0,12,1,1};

WatchTime alarmTime = {0,30,7,1,1};
bool alarmEnabled = true;


volatile uint32_t ms_ticks = 0;

volatile bool g_tick_1s = false;
volatile bool g_force_redraw = true;

uint16_t alarmCounter = 0;

uint8_t get_days_in_month(uint8_t month) {
    switch(month) {
        case 2: return 28;
        case 4: case 6: case 9: case 11: return 30;
        default: return 31;
    }
}

void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void)
{
    _T1IF = 0;

    ms_ticks += 1000;

    now.sec++;
    if (now.sec >= 60) {
        now.sec = 0;
        now.min++;

        if (now.min >= 60) {
            now.min = 0;
            now.hour++;

            if (now.hour >= 24) {
                now.hour = 0;
                now.day++;

                if (now.day > get_days_in_month(now.month)) {
                    now.day = 1;
                    now.month++;

                    if (now.month > 12)
                        now.month = 1;
                }
            }
        }
    }

    g_tick_1s = true;
}

uint8_t get_display_hour(uint8_t hour)
{
    if (myFormat == TIME_24H)
        return hour;

    uint8_t h = hour % 12;
    return (h == 0) ? 12 : h;
}

void check_alarm_timeout(void)
{
    if (myState == STATE_ALARM_RINGING) {
        alarmCounter++;

        if (alarmCounter > 20) {
            myState = STATE_CLOCK;
            alarmCounter = 0;
            g_force_redraw = true;
        }
    }
}