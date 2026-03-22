#include "h/watch.h"
#include "h/menu.h"
#include "h/alarm.h"
#include <stdio.h>
#include <stdbool.h>

/* Preview getter from menu_set_time.c */
void menu_set_time_get_preview(WatchTime* t);

extern bool alarmEnabled;
extern WatchTime alarmTime;
uint8_t get_display_hour(uint8_t hour24);

static WatchTime last_drawn = {255,255,255,255,255};
static AppState  last_state = (AppState)255;
static FaceType  last_face = (FaceType)255;
static bool      last_alarm_enabled = false;
static MenuPage  last_title_page = (MenuPage)255;

/*
 * Small alarm/clock icon that appears near the top.
 */
static void draw_alarm_icon(uint16_t color)
{
    int x = 88;
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

    /* tiny hands in the middle */
    oledC_DrawPoint(x+3, y+3, color);
    oledC_DrawPoint(x+3, y+2, color);
    oledC_DrawPoint(x+4, y+4, color);
}

/*
 * Draws the top bar while inside the menu.
 * Only the changed parts are refreshed to keep the screen stable.
 */
static void draw_menu_top_bar(const WatchTime* t, bool force)
{
    uint16_t bg   = 0x0000;
    uint16_t text = 0xFFFF;
    MenuPage page = menu_get_current_page();
    WatchTime shown_time = *t;

    /* On SET TIME page, show the temporary edited values instead of real now */
    if (page == MENU_PAGE_SET_TIME)
    {
        menu_set_time_get_preview(&shown_time);
    }

    bool page_changed  = (page != last_title_page);
    bool alarm_changed = (alarmEnabled != last_alarm_enabled);
    bool hm_changed    = force ||
                         (shown_time.hour != last_drawn.hour) ||
                         (shown_time.min  != last_drawn.min);
    bool sec_changed   = force ||
                         (shown_time.sec  != last_drawn.sec);

    /* Left title area */
    if (force || page_changed)
    {
        oledC_DrawRectangle(0, 0, 42, 20, bg);

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
                oledC_DrawString(2, 2, 1, 1, (uint8_t*)"DIS", text);
                oledC_DrawString(2, 11, 1, 1, (uint8_t*)"PLAY", text);
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

    /* Alarm icon area */
    if (force || alarm_changed)
    {
        oledC_DrawRectangle(88, 5, 95, 12, bg);

        if (alarmEnabled)
        {
            draw_alarm_icon(text);
        }

        last_alarm_enabled = alarmEnabled;
    }

    /* Hours and minutes */
    if (hm_changed)
    {
        char buf[6];
        oledC_DrawRectangle(40, 8, 71, 18, bg);
        sprintf(buf, "%02d:%02d", get_display_hour(shown_time.hour), shown_time.min);
        oledC_DrawString(40, 8, 1, 1, (uint8_t*)buf, text);
    }

    /* Seconds */
    if (sec_changed)
    {
        char buf[4];
        oledC_DrawRectangle(70, 8, 87, 18, bg);
        sprintf(buf, ":%02d", shown_time.sec);
        oledC_DrawString(70, 8, 1, 1, (uint8_t*)buf, text);
    }

    /* Save only what we actually showed in the top bar */
    last_drawn.hour = shown_time.hour;
    last_drawn.min  = shown_time.min;
    last_drawn.sec  = shown_time.sec;
}

void update_display(void)
{
    uint16_t bg   = 0x0000;
    uint16_t text = 0xFFFF;
    WatchTime t   = now;

    bool state_changed = (myState != last_state);
    bool face_changed  = (myFace  != last_face);

    static bool alarm_screen_drawn = false;

    if (myState == STATE_ALARM_RINGING)
    {
        if (!alarm_screen_drawn)
        {
            char buf[16];

            oledC_DrawRectangle(0, 0, 95, 95, 0x0000);
            sprintf(buf, "%02u:%02u", alarmTime.hour, alarmTime.min);

            oledC_DrawString(18, 28, 2, 2, (uint8_t*)"ALARM", 0xFFFF);
            oledC_DrawString(24, 56, 2, 2, (uint8_t*)buf, 0xFFFF);

            alarm_screen_drawn = true;
        }

        last_state = myState;
        last_face = myFace;
        last_alarm_enabled = alarmEnabled;
        last_drawn = t;
        return;
    }
    else
    {
        alarm_screen_drawn = false;
    }

    /* ---------------- MENU ---------------- */
    if (myState == STATE_MENU)
    {
        bool entering_menu = state_changed;

        if (entering_menu)
        {
            oledC_setBackground(bg);
            oledC_clearScreen();

            /* Reset menu-related drawing cache */
            menu_reset_draw_cache();
            last_title_page = (MenuPage)255;

            /* Invalidate shown top-bar time so first menu draw is forced cleanly */
            last_drawn.hour = 255;
            last_drawn.min  = 255;
            last_drawn.sec  = 255;

            /* Force one full menu draw after entering */
            g_force_redraw = true;
        }

        draw_menu_top_bar(&t, entering_menu || g_force_redraw);

        if (g_force_redraw)
        {
            menu_draw();
            g_force_redraw = false;
        }

        last_state = myState;
        last_face  = myFace;
        return;
    }

    /* ---------------- CLOCK SCREEN ---------------- */
    /*
     * Clear the whole screen when:
     * 1. state changed (menu <-> clock)
     * 2. face changed (digital <-> analog)
     *
     * Without this, old pixels from the previous mode can stay behind.
     */
    if (state_changed || face_changed)
    {
        oledC_setBackground(bg);
        oledC_clearScreen();

        /* Invalidate cache so everything needed gets redrawn */
        last_drawn.hour  = 255;
        last_drawn.min   = 255;
        last_drawn.sec   = 255;
        last_drawn.day   = 255;
        last_drawn.month = 255;
        last_title_page  = (MenuPage)255;
    }

    /* Alarm icon */
    if (state_changed || face_changed || (alarmEnabled != last_alarm_enabled))
    {
        /* Clear the same area where the icon is actually drawn */
        oledC_DrawRectangle(88, 5, 95, 12, bg);

        if (alarmEnabled)
        {
            draw_alarm_icon(text);
        }
    }

    /* AM / PM - only in 12H mode */
    if (state_changed || face_changed || (t.hour != last_drawn.hour))
    {
        oledC_DrawRectangle(2, 84, 20, 94, bg);

        if (myFormat == TIME_12H)
        {
            oledC_DrawString(2, 84, 1, 1,
                             (uint8_t*)((t.hour < 12) ? "am" : "pm"),
                             text);
        }
    }

    /* Date */
    if (state_changed ||
        face_changed  ||
        (t.day   != last_drawn.day) ||
        (t.month != last_drawn.month))
    {
        char date_buf[6];
        oledC_DrawRectangle(66, 84, 95, 94, bg);
        sprintf(date_buf, "%02u/%02u", t.day, t.month);
        oledC_DrawString(66, 84, 1, 1, (uint8_t*)date_buf, text);
    }

    /* ---------------- ANALOG ---------------- */
    if (myFace == FACE_ANALOG)
    {
        if (state_changed || face_changed || (t.sec != last_drawn.sec))
        {
            update_analog_face();
        }

        last_drawn = t;
        last_state = myState;
        last_face  = myFace;
        last_alarm_enabled = alarmEnabled;
        return;
    }

    /* ---------------- DIGITAL ---------------- */
    {
        bool hm_changed = state_changed ||
                          face_changed  ||
                          (t.hour != last_drawn.hour) ||
                          (t.min  != last_drawn.min);

        if (hm_changed)
        {
            char buf[10];
            oledC_DrawRectangle(10, 35, 90, 55, bg);
            sprintf(buf, "%02d:%02d", get_display_hour(t.hour), t.min);
            oledC_DrawString(10, 35, 2, 2, (uint8_t*)buf, text);
        }

        if (state_changed || face_changed || (t.sec != last_drawn.sec))
        {
            char buf[5];
            oledC_DrawRectangle(70, 42, 95, 54, bg);
            sprintf(buf, ":%02d", t.sec);
            oledC_DrawString(70, 42, 1, 1, (uint8_t*)buf, text);
        }
    }

    last_drawn = t;
    last_state = myState;
    last_face  = myFace;
    last_alarm_enabled = alarmEnabled;
}