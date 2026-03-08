#include "h/menu_pages.h"
#include "h/menu.h"
#include "h/watch.h"
#include "h/menu_editor.h"
#include <stdbool.h>
#include <stdint.h>

extern WatchTime alarmTime;
extern bool alarmEnabled;
extern volatile uint16_t g_pot_value;
extern volatile bool g_force_redraw;

static uint8_t alarm_enabled_u8 = 0;

static MenuEditField alarm_fields[] =
{
    {
        "Active", FIELD_TYPE_TOGGLE, &alarm_enabled_u8, 0,
        0, 1, 100,
        8, 30, 40, 52,
        12, 36, 1, 1
    },
    {
        "Hour", FIELD_TYPE_RANGE, (uint8_t*)&alarmTime.hour, 0,
        0, 23, 40,
        8, 56, 40, 78,
        12, 62, 1, 1
    },
    {
        "Min", FIELD_TYPE_RANGE, (uint8_t*)&alarmTime.min, 0,
        0, 59, 15,
        50, 56, 82, 78,
        54, 62, 1, 1
    }
};

static MenuEditorState alarm_editor =
{
    alarm_fields,
    3,
    "BACK",
    25, 80, 70, 95,
    35, 84,
    "ALARM",
    0, -1, 0, 0, 0, "",
    true, -1
};

uint8_t menu_alarm_get_count(void)
{
    return 4;
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

    menu_editor_on_select(&alarm_editor, index, &go_back);

    if (index == 0) {
        alarmEnabled = (alarm_enabled_u8 != 0);
    }

    if (go_back) {
        alarmEnabled = (alarm_enabled_u8 != 0);
        menu_editor_reset(&alarm_editor);
        menu_set_current_page(MENU_PAGE_MAIN);
        return;
    }

    menu_set_cursor((index + 1) % menu_alarm_get_count());
    g_force_redraw = true;
}

void menu_alarm_update_from_pot(void)
{
    if (!alarm_editor.initialized) {
        alarm_enabled_u8 = alarmEnabled ? 1 : 0;
    }

    menu_editor_update_from_pot(&alarm_editor, g_pot_value, menu_get_cursor());
    g_force_redraw = true;
}

void menu_alarm_custom_draw(void)
{
    menu_editor_draw(&alarm_editor, menu_get_cursor(), &g_force_redraw);
}