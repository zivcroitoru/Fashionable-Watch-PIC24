#include "h/menu_pages.h"
#include "h/menu.h"
#include "h/watch.h"
#include "../oledDriver/oledC.h"
#include "../oledDriver/oledC_shapes.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

extern volatile WatchTime now;
extern volatile uint16_t g_pot_value;
extern volatile bool g_force_redraw;

static uint8_t edit_hour = 255;
static uint8_t edit_min  = 255;
static uint8_t edit_sec  = 255;

static int8_t  last_cursor = -1;
static uint16_t pot_start_val = 0;
static int16_t  time_start_val = 0;

static uint32_t msg_timer = 0;
static const char* confirm_msg = "";

/* partial refresh cache */
static bool first_draw = true;
static uint8_t last_drawn_hour = 255;
static uint8_t last_drawn_min  = 255;
static uint8_t last_drawn_sec  = 255;
static int8_t  last_drawn_box  = -1;
static uint32_t last_drawn_msg_timer = 9999;
static void draw_box(uint8_t cursor, uint16_t color)
{
    if (cursor == 0) {   // HOURS
        oledC_DrawLine(8, 33, 35, 33, 1, color);
        oledC_DrawLine(8, 57, 35, 57, 1, color);
        oledC_DrawLine(8, 33, 8, 57, 1, color);
        oledC_DrawLine(35, 33, 35, 57, 1, color);
    }
    else if (cursor == 1) {   // MINUTES
        oledC_DrawLine(38, 33, 65, 33, 1, color);
        oledC_DrawLine(38, 57, 65, 57, 1, color);
        oledC_DrawLine(38, 33, 38, 57, 1, color);
        oledC_DrawLine(65, 33, 65, 57, 1, color);
    }
    else if (cursor == 2) {   // SECONDS
        oledC_DrawLine(68, 40, 95, 40, 1, color);
        oledC_DrawLine(68, 56, 95, 56, 1, color);
        oledC_DrawLine(68, 40, 68, 56, 1, color);
        oledC_DrawLine(95, 40, 95, 56, 1, color);
    }
    else if (cursor == 3) {   // BACK
        oledC_DrawLine(25, 80, 70, 80, 1, color);
        oledC_DrawLine(25, 95, 70, 95, 1, color);
        oledC_DrawLine(25, 80, 25, 95, 1, color);
        oledC_DrawLine(70, 80, 70, 95, 1, color);
    }
}

void menu_set_time_reset_state(void)
{
    edit_hour = 255;
    edit_min  = 255;
    edit_sec  = 255;

    last_cursor = -1;
    pot_start_val = 0;
    time_start_val = 0;

    msg_timer = 0;
    confirm_msg = "";

    first_draw = true;
    last_drawn_hour = 255;
    last_drawn_min  = 255;
    last_drawn_sec  = 255;
    last_drawn_box  = -1;
    last_drawn_msg_timer = 9999;
}

uint8_t menu_set_time_get_count(void)
{
    return 4;   /* 0=hours, 1=minutes, 2=seconds, 3=back */
}

const char* menu_set_time_get_item(uint8_t index)
{
    (void)index;
    return "";
}

void menu_set_time_on_select(uint8_t index)
{
    if (index == 0) {
        now.hour = edit_hour;
        confirm_msg = "updated hours";
    }
    else if (index == 1) {
        now.min = edit_min;
        confirm_msg = "updated minutes";
    }
    else if (index == 2) {
        now.sec = edit_sec;
        confirm_msg = "updated seconds";
    }
    else if (index == 3) {
        menu_set_time_reset_state();
        menu_set_current_page(MENU_PAGE_MAIN);
        return;
    }

    msg_timer = 100;
    g_force_redraw = true;

    /* hours -> minutes -> seconds -> back */
    menu_set_cursor((index) % menu_set_time_get_count());
}

void menu_set_time_update_from_pot(void)
{
    uint16_t current_pot = g_pot_value;
    int8_t cursor = menu_get_cursor();

    if (edit_hour == 255 || edit_min == 255 || edit_sec == 255) {
        edit_hour = now.hour;
        edit_min  = now.min;
        edit_sec  = now.sec;

        last_cursor = cursor;
        pot_start_val = current_pot;

        if (cursor == 0)      time_start_val = edit_hour;
        else if (cursor == 1) time_start_val = edit_min;
        else if (cursor == 2) time_start_val = edit_sec;

        g_force_redraw = true;
        return;
    }

    if (cursor != last_cursor) {
        last_cursor = cursor;
        pot_start_val = current_pot;

        if (cursor == 0)      time_start_val = edit_hour;
        else if (cursor == 1) time_start_val = edit_min;
        else if (cursor == 2) time_start_val = edit_sec;

        g_force_redraw = true;
        return;
    }

    if (cursor == 0) {
        int16_t offset = ((int16_t)current_pot - (int16_t)pot_start_val) / 40;
        int16_t new_h = (time_start_val + offset) % 24;
        if (new_h < 0) new_h += 24;

        if ((uint8_t)new_h != edit_hour) {
            edit_hour = (uint8_t)new_h;
            g_force_redraw = true;
        }
    }
    else if (cursor == 1) {
        int16_t offset = ((int16_t)current_pot - (int16_t)pot_start_val) / 15;
        int16_t new_m = (time_start_val + offset) % 60;
        if (new_m < 0) new_m += 60;

        if ((uint8_t)new_m != edit_min) {
            edit_min = (uint8_t)new_m;
            g_force_redraw = true;
        }
    }
    else if (cursor == 2) {
        int16_t offset = ((int16_t)current_pot - (int16_t)pot_start_val) / 15;
        int16_t new_s = (time_start_val + offset) % 60;
        if (new_s < 0) new_s += 60;

        if ((uint8_t)new_s != edit_sec) {
            edit_sec = (uint8_t)new_s;
            g_force_redraw = true;
        }
    }
}

void menu_set_time_custom_draw(void)
{
    uint16_t text = 0xFFFF;
    uint16_t bg   = 0x0000;
    int8_t cursor = menu_get_cursor();
    char buf[16];

    if (edit_hour == 255 || edit_min == 255 || edit_sec == 255) {
        edit_hour = now.hour;
        edit_min  = now.min;
        edit_sec  = now.sec;
    }

    if (first_draw) {
        oledC_DrawRectangle(0, 24, 95, 95, bg);

        /* static parts */
        oledC_DrawRectangle(34, 35, 39, 55, bg);
        oledC_DrawString(34, 35, 2, 2, (uint8_t*)" ", text);

        oledC_DrawRectangle(25, 80, 70, 95, bg);
        oledC_DrawString(35, 84, 1, 1, (uint8_t*)"BACK", text);

        last_drawn_hour = 255;
        last_drawn_min  = 255;
        last_drawn_sec  = 255;
        last_drawn_box  = -1;
        last_drawn_msg_timer = 9999;
        first_draw = false;
    }

    /* hours */
    if (edit_hour != last_drawn_hour) {
        oledC_DrawRectangle(10, 35, 33, 55, bg);
        sprintf(buf, "%02u", edit_hour);
        oledC_DrawString(10, 35, 2, 2, (uint8_t*)buf, text);
        last_drawn_hour = edit_hour;
    }

    /* minutes */
    if (edit_min != last_drawn_min) {
        oledC_DrawRectangle(40, 35, 63, 55, bg);
        sprintf(buf, "%02u", edit_min);
        oledC_DrawString(40, 35, 2, 2, (uint8_t*)buf, text);
        last_drawn_min = edit_min;
    }

    /* seconds */
    if (edit_sec != last_drawn_sec) {
        oledC_DrawRectangle(70, 42, 95, 54, bg);
        sprintf(buf, "%02u", edit_sec);
        oledC_DrawString(70, 42, 1, 1, (uint8_t*)buf, text);
        last_drawn_sec = edit_sec;
    }

    /* message */
    if (msg_timer != last_drawn_msg_timer) {
        oledC_DrawRectangle(10, 65, 95, 75, bg);

        if (msg_timer > 0) {
            oledC_DrawString(10, 65, 1, 1, (uint8_t*)confirm_msg, text);
        }

        last_drawn_msg_timer = msg_timer;
    }

    if (msg_timer > 0) {
        msg_timer--;
        g_force_redraw = true;
    }

    /* selection box */
    if (cursor != last_drawn_box) {
        if (last_drawn_box >= 0) {
            draw_box((uint8_t)last_drawn_box, bg);

            /* redraw back text after erasing its border */
            if (last_drawn_box == 3) {
                oledC_DrawRectangle(25, 80, 70, 95, bg);
                oledC_DrawString(35, 84, 1, 1, (uint8_t*)"BACK", text);
            }
        }

        draw_box((uint8_t)cursor, text);
        last_drawn_box = cursor;
    }
}