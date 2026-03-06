#include "h/watch.h"
#include <stdio.h>
#include "h/menu.h"

#define MENU_CLOCK_X 46
#define MENU_CLOCK_Y 12

static WatchTime last_drawn = {255,255,255,255,255};
static AppState last_state = (AppState)255;

void update_display(void)
{
    uint16_t bg = 0x0000;
    uint16_t text = 0xFFFF;
    WatchTime t = now;

    bool state_changed = (myState != last_state);

    // --- CASE 1: MENU ---
    if (myState == STATE_MENU)
    {
        bool entering_menu = state_changed;
        bool menu_clock_changed = (t.hour != last_drawn.hour) ||
                                  (t.min  != last_drawn.min)  ||
                                  (t.sec  != last_drawn.sec);

        // Clear the full screen only once when entering the menu
    if (entering_menu) {
        oledC_DrawRectangle(0, 0, 95, 95, bg);
        menu_reset_draw_cache();
        g_force_redraw = true;
    }

        // Redraw the small menu clock only when time changes
        if (entering_menu || menu_clock_changed) {
            char buf[9];
            oledC_DrawRectangle(MENU_CLOCK_X, MENU_CLOCK_Y,
                                MENU_CLOCK_X + 50, MENU_CLOCK_Y + 10, bg);
            sprintf(buf, "%02d:%02d:%02d",
                    get_display_hour(t.hour), t.min, t.sec);
            oledC_DrawString(MENU_CLOCK_X, MENU_CLOCK_Y, 1, 1,
                             (uint8_t*)buf, text);
        }

        // Redraw the menu list only when forced (entering menu / cursor move)
        if (g_force_redraw) {
            menu_draw();
            g_force_redraw = false;
        }

        last_drawn = t;
        last_state = myState;
        return;   // IMPORTANT: do not fall through to the clock face code
    }

    // If we just left the menu, force a clean redraw of the watch face once
    if (state_changed) {
        oledC_DrawRectangle(0, 0, 95, 95, bg);
        last_drawn.hour = 255;
        last_drawn.min  = 255;
        last_drawn.sec  = 255;
        last_drawn.day  = 255;
        last_drawn.month= 255;
    }

    // --- CASE 2: ANALOG FACE ---
    if (myFace == FACE_ANALOG)
    {
        if (state_changed || t.sec != last_drawn.sec) {
            oledC_DrawRectangle(0, 0, 95, 95, bg);
            oledC_DrawString(10, 40, 1, 1, (uint8_t*)"ANALOG", text);
            // Draw clock hands here
        }

        last_drawn = t;
        last_state = myState;
        return;
    }

    // --- CASE 3: DIGITAL FACE ---
    bool hm_changed = state_changed ||
                      (t.hour != last_drawn.hour) ||
                      (t.min  != last_drawn.min);

    if (hm_changed) {
        char buf[10];
        oledC_DrawRectangle(10, 35, 90, 55, bg);
        sprintf(buf, "%02d:%02d", get_display_hour(t.hour), t.min);
        oledC_DrawString(10, 35, 2, 2, (uint8_t*)buf, text);
    }

    if (state_changed || t.sec != last_drawn.sec) {
        char buf[5];
        oledC_DrawRectangle(70, 42, 95, 54, bg);
        sprintf(buf, ":%02d", t.sec);
        oledC_DrawString(70, 42, 1, 1, (uint8_t*)buf, text);
    }

    last_drawn = t;
    last_state = myState;
}
