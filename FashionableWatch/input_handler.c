#include "h/watch.h"
#include "h/menu.h"
#include <xc.h>
#include "./../Accel_i2c.h"
#include "h/alarm.h"

#define LONG_PRESS_MS 2000  // 2 seconds = long press

extern volatile AppState myState;
extern volatile uint32_t ms_ticks; // system time in milliseconds

// Track button state (pressed / released + when it started)
typedef struct
{
    bool is_down;
    uint32_t start_time;
} BtnStatus;

// Internal button tracking
static BtnStatus s1_status = {false, 0};
static BtnStatus s2_status = {false, 0};

void check_inputs(void)
{
    // Buttons are ACTIVE LOW (pressed = 0)
    bool s1_active = (PORTAbits.RA11 == 0); // S1
    bool s2_active = (PORTAbits.RA12 == 0); // S2

    // ================= S1 =================
    // Detect button press (edge: up -> down)
    if (s1_active && !s1_status.is_down)
    {
        s1_status.is_down = true;
        s1_status.start_time = ms_ticks; // start timing press
        LATAbits.LATA0 = 1; // turn ON LED (visual feedback)

        // If alarm is ringing -> stop it immediately
        if (alarm_is_ringing()) {
            alarm_stop();
            return;
        }
    }
    // Detect button release (edge: down -> up)
    else if (!s1_active && s1_status.is_down)
    {
        uint32_t duration = ms_ticks - s1_status.start_time;
        s1_status.is_down = false;
        LATAbits.LATA0 = 0; // turn OFF LED

        // Long press ? enter/exit menu
        if (duration >= LONG_PRESS_MS)
        {
            if (myState == STATE_CLOCK)
            {
                menu_enter(); // go into menu
            }
            else if (myState == STATE_MENU)
            {
                menu_exit(); // go back to clock
            }
        }
        // Short press -> navigate menu
        else
        {
            if (myState == STATE_MENU)
            {
                menu_next_item(); // move selection
            }
        }
    }

    // ================= S2 =================
    // Detect press
    if (s2_active && !s2_status.is_down)
    {
        s2_status.is_down = true;
        s2_status.start_time = ms_ticks;
        LATAbits.LATA1 = 1; // LED ON

        // Stop alarm if ringing
        if (alarm_is_ringing()) {
            alarm_stop();
            return;
        }
    }
    // Detect release
    else if (!s2_active && s2_status.is_down)
    {
        s2_status.is_down = false;
        LATAbits.LATA1 = 0; // LED OFF

        // In menu ? confirm/select
        if (myState == STATE_MENU)
        {
            menu_select_current();
        }
    }
}

// Detect face-down gesture using accelerometer
void check_gestures(void)
{
    static uint8_t face_down_latched = 0; // prevents repeated triggers
    uint8_t is_face_down_now = accel_is_face_down();

    if (is_face_down_now)
    {
        // Trigger only once when flipping to face-down
        if (!face_down_latched)
        {
            face_down_latched = 1;

            // Stop alarm if active
            if (alarm_is_ringing()) {
                alarm_stop();
            }

            // Exit menu if currently inside
            if (myState == STATE_MENU)
            {
                menu_exit();
            }
        }
    }
    else
    {
        // Reset latch when device is flipped back
        face_down_latched = 0;
    }
}