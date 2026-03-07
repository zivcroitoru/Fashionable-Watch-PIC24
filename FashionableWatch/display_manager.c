#include "h/watch.h"
#include "h/menu.h"
#include <stdio.h>
#include <stdbool.h>

#define MENU_CLOCK_X 46
#define MENU_CLOCK_Y 12

extern bool alarmEnabled;

static WatchTime last_drawn = {255,255,255,255,255};
static AppState last_state = (AppState)255;


void draw_alarm_star(uint16_t color)
{
    // center at (6,6)
    oledC_DrawPoint(6,2,color);
    oledC_DrawPoint(5,3,color);
    oledC_DrawPoint(7,3,color);
    oledC_DrawPoint(4,4,color);
    oledC_DrawPoint(8,4,color);

    oledC_DrawPoint(6,5,color);

    oledC_DrawPoint(4,6,color);
    oledC_DrawPoint(8,6,color);

    oledC_DrawPoint(5,7,color);
    oledC_DrawPoint(7,7,color);

    oledC_DrawPoint(6,8,color);
}
void update_display(void)
{
    uint16_t bg = 0x0000;
    uint16_t text = 0xFFFF;
    WatchTime t = now;

    bool state_changed = (myState != last_state);

    // ---------------- MENU ----------------
    if (myState == STATE_MENU)
    {
        bool entering_menu = state_changed;
        bool menu_clock_changed = (t.hour != last_drawn.hour) ||
                                  (t.min  != last_drawn.min)  ||
                                  (t.sec  != last_drawn.sec);

        if (entering_menu)
        {
            oledC_DrawRectangle(0, 0, 95, 95, bg);
            menu_reset_draw_cache();
            g_force_redraw = true;
        }

        if (entering_menu || menu_clock_changed)
        {
            char buf[9];

            oledC_DrawRectangle(
                MENU_CLOCK_X,
                MENU_CLOCK_Y,
                MENU_CLOCK_X + 50,
                MENU_CLOCK_Y + 10,
                bg
            );

            sprintf(buf, "%02d:%02d:%02d",
                    get_display_hour(t.hour), t.min, t.sec);

            oledC_DrawString(
                MENU_CLOCK_X,
                MENU_CLOCK_Y,
                1,
                1,
                (uint8_t*)buf,
                text
            );
        }

        if (g_force_redraw)
        {
            menu_draw();
            g_force_redraw = false;
        }

        last_drawn = t;
        last_state = myState;
        return;
    }

    // ---------------- LEFT MENU -> CLOCK ----------------
    if (state_changed)
    {
        oledC_DrawRectangle(0, 0, 95, 95, bg);
        last_drawn.hour  = 255;
        last_drawn.min   = 255;
        last_drawn.sec   = 255;
        last_drawn.day   = 255;
        last_drawn.month = 255;
    }

    // ---------------- COMMON WIDGETS (NOT MENU) ----------------

    // Alarm at top-left
// Alarm icon (top-left)
if (state_changed)
{
    oledC_DrawRectangle(0,0,14,14,bg);

    if (alarmEnabled)
    {
        draw_alarm_star(text);
    }
}

    // AM / PM at bottom-left
    if (state_changed || t.hour != last_drawn.hour)
    {
        oledC_DrawRectangle(2, 84, 20, 94, bg);
        oledC_DrawString(
            2,
            84,
            1,
            1,
            (uint8_t*)((t.hour < 12) ? "am" : "pm"),
            text
        );
    }

    // Date at bottom-right
    if (state_changed ||
        t.day   != last_drawn.day ||
        t.month != last_drawn.month)
    {
        char date_buf[6];
        oledC_DrawRectangle(66, 84, 95, 94, bg);
        sprintf(date_buf, "%02u/%02u", t.day, t.month);
        oledC_DrawString(66, 84, 1, 1, (uint8_t*)date_buf, text);
    }

    // ---------------- ANALOG ----------------
    if (myFace == FACE_ANALOG)
    {
        if (state_changed || t.sec != last_drawn.sec)
        {
            oledC_DrawRectangle(20, 30, 76, 50, bg);
            oledC_DrawString(10, 40, 1, 1, (uint8_t*)"ANALOG", text);
            // draw analog hands here
        }

        last_drawn = t;
        last_state = myState;
        return;
    }

    // ---------------- DIGITAL ----------------
    {
        bool hm_changed = state_changed ||
                          (t.hour != last_drawn.hour) ||
                          (t.min  != last_drawn.min);

        if (hm_changed)
        {
            char buf[10];
            oledC_DrawRectangle(10, 35, 90, 55, bg);
            sprintf(buf, "%02d:%02d", get_display_hour(t.hour), t.min);
            oledC_DrawString(10, 35, 2, 2, (uint8_t*)buf, text);
        }

        if (state_changed || t.sec != last_drawn.sec)
        {
            char buf[5];
            oledC_DrawRectangle(70, 42, 95, 54, bg);
            sprintf(buf, ":%02d", t.sec);
            oledC_DrawString(70, 42, 1, 1, (uint8_t*)buf, text);
        }
    }

    last_drawn = t;
    last_state = myState;
}