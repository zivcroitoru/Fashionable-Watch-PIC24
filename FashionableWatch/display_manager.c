#include "h/watch.h"
#include "h/menu.h"
#include <stdio.h>
#include <stdbool.h>

#define MENU_CLOCK_X 46
#define MENU_CLOCK_Y 12

extern bool alarmEnabled;

static WatchTime last_drawn = {255,255,255,255,255};
static AppState last_state = (AppState)255;
static bool last_alarm_enabled = false;
static MenuPage last_title_page = (MenuPage)255;

void draw_alarm_star(uint16_t color)
{
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

static void draw_menu_title(void)
{
    uint16_t bg = 0x0000;
    uint16_t text = 0xFFFF;
    MenuPage page = menu_get_current_page();

    if (page == last_title_page)
        return;

    // clear only the title area, not the whole top bar
    oledC_DrawRectangle(0, 0, 40, 20, bg);
    oledC_DrawString(2, 2, 1, 1, (uint8_t*)menu_get_title(), text);

    last_title_page = page;
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
    bool hm_changed = entering_menu ||
                      (t.hour != last_drawn.hour) ||
                      (t.min  != last_drawn.min);

    bool sec_changed = entering_menu ||
                       (t.sec != last_drawn.sec);

    if (entering_menu)
    {
        oledC_DrawRectangle(0, 0, 95, 95, bg);
        menu_reset_draw_cache();
        last_title_page = (MenuPage)255;
        g_force_redraw = true;
    }

    if (entering_menu || (alarmEnabled != last_alarm_enabled))
    {
        oledC_DrawRectangle(0, 0, 14, 14, bg);

        if (alarmEnabled)
        {
            draw_alarm_star(text);
        }
    }

    // draw title first, because it clears the top bar
    draw_menu_title();

    if (hm_changed)
    {
        char buf[6];
        oledC_DrawRectangle(38, 8, 81, 18, bg);
        sprintf(buf, "%02d:%02d", get_display_hour(t.hour), t.min);
        oledC_DrawString(48, 8, 1, 1, (uint8_t*)buf, text);
    }

    if (sec_changed)
    {
        char buf[5];
        oledC_DrawRectangle(82, 8, 95, 18, bg);
        sprintf(buf, ":%02d", t.sec);
        oledC_DrawString(78, 8, 1, 1, (uint8_t*)buf, text);
    }

    if (g_force_redraw)
    {
        menu_draw();
        g_force_redraw = false;
    }

    last_drawn = t;
    last_state = myState;
    last_alarm_enabled = alarmEnabled;
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
        last_title_page  = (MenuPage)255;
    }

    // Alarm icon
    if (state_changed || (alarmEnabled != last_alarm_enabled))
    {
        oledC_DrawRectangle(0, 0, 14, 14, bg);

        if (alarmEnabled)
        {
            draw_alarm_star(text);
        }
    }

    // AM / PM
    if (state_changed || t.hour != last_drawn.hour)
    {
        oledC_DrawRectangle(2, 84, 20, 94, bg);
        oledC_DrawString(2, 84, 1, 1,
                         (uint8_t*)((t.hour < 12) ? "am" : "pm"),
                         text);
    }

    // Date
    if (state_changed ||
        t.day   != last_drawn.day ||
        t.month != last_drawn.month)
    {
        char date_buf[6];
        oledC_DrawRectangle(66, 84, 95, 94, bg);
        sprintf(date_buf, "%02u/%02u", t.day, t.month);
        oledC_DrawString(66, 84, 1, 1, (uint8_t*)date_buf, text);
    }

    // ANALOG
    if (myFace == FACE_ANALOG)
    {
        if (state_changed || t.sec != last_drawn.sec)
        {
            oledC_DrawRectangle(20, 30, 76, 50, bg);
            oledC_DrawString(10, 40, 1, 1, (uint8_t*)"ANALOG", text);
        }

        last_drawn = t;
        last_state = myState;
        last_alarm_enabled = alarmEnabled;
        return;
    }

    // DIGITAL
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
    last_alarm_enabled = alarmEnabled;
}