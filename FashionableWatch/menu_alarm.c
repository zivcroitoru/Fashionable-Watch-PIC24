#include "h/menu_pages.h"
#include "h/menu.h"
#include "h/watch.h"
#include "h/menu_editor.h"
#include "../oledDriver/oledC.h"
#include "../oledDriver/oledC_shapes.h"
#include <stdbool.h>
#include <stdint.h>

extern WatchTime alarmTime;
extern bool alarmEnabled;
extern volatile uint16_t g_pot_value;
extern volatile bool g_force_redraw;

#define COLOR_TEXT 0xFFFF
#define COLOR_BG   0x0000

// OFF button box
#define OFF_X1  4
#define OFF_Y1  42
#define OFF_X2  28
#define OFF_Y2  64
#define OFF_TX  8
#define OFF_TY  48

static void draw_box(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color)
{
    oledC_DrawLine(x1, y1, x2, y1, 1, color);
    oledC_DrawLine(x1, y2, x2, y2, 1, color);
    oledC_DrawLine(x1, y1, x1, y2, 1, color);
    oledC_DrawLine(x2, y1, x2, y2, 1, color);
}

static MenuEditField alarm_fields[] =
{
    {
        "Hour", FIELD_TYPE_RANGE, (uint8_t*)&alarmTime.hour, 0,
        0, 23, 40,
        34, 42, 58, 64,
        38, 48, 1, 1
    },
    {
        "Min", FIELD_TYPE_RANGE, (uint8_t*)&alarmTime.min, 0,
        0, 59, 15,
        64, 42, 88, 64,
        68, 48, 1, 1
    }
};

static MenuEditorState alarm_editor =
{
    alarm_fields,
    2,
    "BACK",
    68, 80, 95, 95,
    70, 84,
    "",
    0, -1, 0, 0, 0, "",
    true, -1, true
};

uint8_t menu_alarm_get_count(void)
{
    // If alarm is enabled:
    // OFF + Hour + Min + BACK = 4
    //
    // If alarm is disabled:
    // Hour + Min + BACK = 3
    return alarmEnabled ? 4 : 3;
}

const char* menu_alarm_get_item(uint8_t index)
{
    (void)index;
    return "";
}

void menu_alarm_reset_state(void)
{
    menu_editor_reset(&alarm_editor);
}

void menu_alarm_on_select(uint8_t index)
{
    bool go_back = false;

    if (alarmEnabled) {
        // Enabled layout:
        // 0 = OFF
        // 1 = Hour
        // 2 = Min
        // 3 = BACK

        if (index == 0) {
            alarmEnabled = false;

            // After OFF disappears, valid items are:
            // 0 = Hour, 1 = Min, 2 = BACK
            menu_set_cursor(0);
            g_force_redraw = true;
            return;
        }

        if (index == 3) {
            menu_editor_reset(&alarm_editor);
            menu_set_current_page(MENU_PAGE_MAIN);
            g_force_redraw = true;
            return;
        }

        // 1 -> Hour(editor 0), 2 -> Min(editor 1)
        menu_editor_on_select(&alarm_editor, index - 1, &go_back);

        if (index == 1 || index == 2) {
            alarmEnabled = true;
        }
    } else {
        // Disabled layout:
        // 0 = Hour
        // 1 = Min
        // 2 = BACK

        if (index == 2) {
            menu_editor_reset(&alarm_editor);
            menu_set_current_page(MENU_PAGE_MAIN);
            g_force_redraw = true;
            return;
        }

        // 0 -> Hour(editor 0), 1 -> Min(editor 1)
        menu_editor_on_select(&alarm_editor, index, &go_back);

        // Confirming Hour or Min enables alarm automatically
        if (index == 0 || index == 1) {
            alarmEnabled = true;
        }
    }

    menu_set_cursor((index + 1) % menu_alarm_get_count());
    g_force_redraw = true;
}

void menu_alarm_update_from_pot(void)
{
    int8_t cursor = menu_get_cursor();

    if (alarmEnabled) {
        // Enabled layout:
        // 0 = OFF, 1 = Hour, 2 = Min, 3 = BACK
        if (cursor == 1 || cursor == 2) {
            (void)menu_editor_update_from_pot(&alarm_editor, g_pot_value, cursor - 1);
        }
    } else {
        // Disabled layout:
        // 0 = Hour, 1 = Min, 2 = BACK
        if (cursor == 0 || cursor == 1) {
            (void)menu_editor_update_from_pot(&alarm_editor, g_pot_value, cursor);
        }
    }
}

void menu_alarm_custom_draw(void)
{
    int8_t cursor = menu_get_cursor();

    if (alarmEnabled) {
        // Enabled layout:
        // 0 = OFF
        // 1 = Hour -> editor 0
        // 2 = Min  -> editor 1
        // 3 = BACK -> editor back

        if (cursor == 1) {
            menu_editor_draw(&alarm_editor, 0, &g_force_redraw);
        } else if (cursor == 2) {
            menu_editor_draw(&alarm_editor, 1, &g_force_redraw);
        } else {
            menu_editor_draw(&alarm_editor, alarm_editor.field_count, &g_force_redraw);
        }

        // Draw OFF button
        oledC_DrawRectangle(OFF_X1 + 2, OFF_Y1 + 2, OFF_X2 - 2, OFF_Y2 - 2, COLOR_BG);
        oledC_DrawString(OFF_TX, OFF_TY, 1, 1, (uint8_t*)"OFF", COLOR_TEXT);

        if (cursor == 0) {
            draw_box(OFF_X1, OFF_Y1, OFF_X2, OFF_Y2, COLOR_TEXT);
        } else {
            draw_box(OFF_X1, OFF_Y1, OFF_X2, OFF_Y2, COLOR_BG);
        }
    } else {
        // Disabled layout:
        // 0 = Hour -> editor 0
        // 1 = Min  -> editor 1
        // 2 = BACK -> editor back

        if (cursor == 0) {
            menu_editor_draw(&alarm_editor, 0, &g_force_redraw);
        } else if (cursor == 1) {
            menu_editor_draw(&alarm_editor, 1, &g_force_redraw);
        } else {
            menu_editor_draw(&alarm_editor, alarm_editor.field_count, &g_force_redraw);
        }

        // Clear OFF area when alarm is disabled
        oledC_DrawRectangle(OFF_X1, OFF_Y1, OFF_X2, OFF_Y2, COLOR_BG);
    }
}