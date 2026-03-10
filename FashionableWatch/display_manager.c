#include "h/watch.h"
#include "h/menu.h"
#include "h/alarm.h"
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
    int x = 36;
    int y = 5;

    oledC_DrawPoint(x+2, y+0, color);
    oledC_DrawPoint(x+3, y+0, color);
    oledC_DrawPoint(x+4, y+0, color);

    oledC_DrawPoint(x+1, y+1, color);
    oledC_DrawPoint(x+5, y+1, color);

    oledC_DrawPoint(x+0, y+2, color);
    oledC_DrawPoint(x+6, y+2, color);

    oledC_DrawPoint(x+0, y+3, color);
    oledC_DrawPoint(x+6, y+3, color);

    oledC_DrawPoint(x+0, y+4, color);
    oledC_DrawPoint(x+6, y+4, color);

    oledC_DrawPoint(x+1, y+5, color);
    oledC_DrawPoint(x+5, y+5, color);

    oledC_DrawPoint(x+2, y+6, color);
    oledC_DrawPoint(x+3, y+6, color);
    oledC_DrawPoint(x+4, y+6, color);

    /* hands */
    oledC_DrawPoint(x+3, y+3, color);  // center
    oledC_DrawPoint(x+3, y+2, color);  // hour hand
    oledC_DrawPoint(x+4, y+4, color);  // minute hand
}

static void draw_menu_top_bar(const WatchTime* t, bool force)
{
    uint16_t bg = 0x0000;
    uint16_t text = 0xFFFF;
    MenuPage page = menu_get_current_page();

    bool page_changed = (page != last_title_page);
    bool alarm_changed = (alarmEnabled != last_alarm_enabled);
    bool hm_changed = force ||
                      (t->hour != last_drawn.hour) ||
                      (t->min  != last_drawn.min);
    bool sec_changed = force ||
                       (t->sec != last_drawn.sec);

    // left title area
    if (force || page_changed)
    {
        oledC_DrawRectangle(0, 0, 35, 20, bg);

        switch (page)
        {
            case MENU_PAGE_MAIN:
                oledC_DrawString(2, 2, 1, 1, (uint8_t*)"MAIN", text);
                oledC_DrawString(2, 11, 1, 1, (uint8_t*)"MENU", text);
                break;

            case MENU_PAGE_SET_TIME:
                oledC_DrawString(2, 2, 1, 1, (uint8_t*)"SET", text);
                oledC_DrawString(2, 11, 1, 1, (uint8_t*)"TIME", text);
                break;

            case MENU_PAGE_SET_DATE:
                oledC_DrawString(2, 2, 1, 1, (uint8_t*)"SET", text);
                oledC_DrawString(2, 11, 1, 1, (uint8_t*)"DATE", text);
                break;

            case MENU_PAGE_ALARM:
                oledC_DrawString(2, 2, 1, 1, (uint8_t*)"SET", text);
                oledC_DrawString(2, 11, 1, 1, (uint8_t*)"ALARM", text);
                break;

            case MENU_PAGE_DISPLAY:
                oledC_DrawString(2, 6, 1, 1, (uint8_t*)"DISPLAY", text);
                break;

            case MENU_PAGE_FORMAT:
                oledC_DrawString(2, 6, 1, 1, (uint8_t*)"FORMAT", text);
                break;

            case MENU_PAGE_ANALOG_THEME:
                oledC_DrawString(2, 2, 1, 1, (uint8_t*)"ANALOG", text);
                oledC_DrawString(2, 11, 1, 1, (uint8_t*)"THEME", text);
                break;

            default:
                oledC_DrawString(2, 6, 1, 1, (uint8_t*)"MENU", text);
                break;
        }

        last_title_page = page;
    }

    // alarm icon area
    if (force || alarm_changed)
    {
        oledC_DrawRectangle(36, 5, 43, 12, bg);

        if (alarmEnabled)
            draw_alarm_star(text);

        last_alarm_enabled = alarmEnabled;
    } 
    // HH:MM
    if (hm_changed)
    {
        char buf[6];
        oledC_DrawRectangle(46, 8, 77, 18, bg);
        sprintf(buf, "%02d:%02d", get_display_hour(t->hour), t->min);
        oledC_DrawString(46, 8, 1, 1, (uint8_t*)buf, text);
    }

    // SS
 if (sec_changed)
    {
        char buf[4]; // Size increased for the colon and null-terminator
        oledC_DrawRectangle(76, 8, 94, 18, bg); // Shifted X left to 76 to reduce space
        sprintf(buf, ":%02d", t->sec); // Added colon
        oledC_DrawString(76, 8, 1, 1, (uint8_t*)buf, text); // Shifted X left to 76
    }
}
void update_display(void)
{
    if (alarm_is_ringing()) {
    alarm_draw_if_active();
    last_state = myState;
    return;
}
    uint16_t bg = 0x0000;
    uint16_t text = 0xFFFF;
    WatchTime t = now;

    bool state_changed = (myState != last_state);

    // ---------------- MENU ----------------
if (myState == STATE_MENU)
{
    bool entering_menu = state_changed;

    if (entering_menu)
    {
        oledC_setBackground(bg);
        oledC_clearScreen();

        menu_reset_draw_cache();
        last_title_page = (MenuPage)255;
        g_force_redraw = true;
    }

    draw_menu_top_bar(&t, entering_menu);

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
        oledC_setBackground(bg);
        oledC_clearScreen();
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
            // Erase the placeholder rectangle and string!
            // Call your new function instead:
            update_analog_face(); 
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