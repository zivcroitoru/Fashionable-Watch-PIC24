#include "h/watch.h"
#include "h/menu.h"
#include <xc.h>

#define LONG_PRESS_MS 2000

extern int8_t menu_cursor_index;
extern volatile AppState myState;
extern volatile FaceType myFace;
extern volatile TimeFormat myFormat;
extern volatile bool g_force_redraw;
extern volatile uint32_t ms_ticks;
extern bool alarmEnabled;

typedef struct {
    bool is_down;
    uint32_t start_time;
} BtnStatus;

static BtnStatus s1_status = {false, 0};
static BtnStatus s2_status = {false, 0};

static void handle_menu_select(void)
{
    switch (menu_cursor_index)
    {
        case 0: // Display Mode
            myFace = (myFace == FACE_DIGI) ? FACE_ANALOG : FACE_DIGI;
            g_force_redraw = true;
            break;

        case 1: // 12H/24H Interval
            myFormat = (myFormat == TIME_12H) ? TIME_24H : TIME_12H;
            g_force_redraw = true;
            break;

        case 2: // Set Time
            // Placeholder for future time-setting screen
            break;

        case 3: // Set Date
            // Placeholder for future date-setting screen
            break;

        case 4: // Alarm
            alarmEnabled = !alarmEnabled;
            g_force_redraw = true;
            break;

        default:
            break;
    }
}

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
            // Long S1: enter menu from clock / exit menu to clock
            if (myState == STATE_CLOCK)
            {
                myState = STATE_MENU;
                menu_cursor_index = 0;
                g_force_redraw = true;
            }
            else if (myState == STATE_MENU)
            {
                myState = STATE_CLOCK;
                g_force_redraw = true;
            }
        }
        else
        {
            // Short S1: arrow down in menu
            if (myState == STATE_MENU)
            {
                menu_cursor_index++;
                if (menu_cursor_index >= 5)
                    menu_cursor_index = 0;

                g_force_redraw = true;
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

        // Short S2: select current menu item
        if (myState == STATE_MENU)
        {
            handle_menu_select();
        }
    }
}

void check_gestures(void)
{
    // Placeholder
}