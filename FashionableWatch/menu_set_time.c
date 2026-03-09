#include "h/menu_pages.h"
#include "h/menu.h"
#include "h/watch.h"
#include "h/menu_editor.h"
#include <stdbool.h>
#include <stdint.h>

extern volatile WatchTime now;
extern volatile uint16_t g_pot_value;
extern volatile bool g_force_redraw;

static MenuEditField time_fields[] =
{
    {
        "Hour", FIELD_TYPE_RANGE, (uint8_t*)&now.hour, 0,
        0, 23, 40,
        8, 33, 35, 57,
        10, 35, 2, 2
    },
    {
        "Min", FIELD_TYPE_RANGE, (uint8_t*)&now.min, 0,
        0, 59, 15,
        38, 33, 65, 57,
        40, 35, 2, 2
    },
    {
        "Sec", FIELD_TYPE_RANGE, (uint8_t*)&now.sec, 0,
        0, 59, 15,
        68, 40, 95, 56,
        70, 42, 1, 1
    }
};

static MenuEditorState time_editor =
{
    time_fields,
    3,
    "BACK",
    25, 80, 70, 95,
    35, 84,
    "SET TIME",
    0, -1, 0, 0, 0, "",
    true, -1
};

uint8_t menu_set_time_get_count(void)
{
    return 4; /* 3 fields + back */
}

const char* menu_set_time_get_item(uint8_t index)
{
    (void)index;
    return "";
}

void menu_set_time_reset_state(void)
{
    menu_editor_reset(&time_editor);
}

void menu_set_time_on_select(uint8_t index)
{
    bool go_back = false;

    menu_editor_on_select(&time_editor, index, &go_back);

    if (go_back) {
        menu_editor_reset(&time_editor);
        menu_set_current_page(MENU_PAGE_MAIN);
        return;
    }

    menu_set_cursor((index + 1) % menu_set_time_get_count());
    g_force_redraw = true;
}

void menu_set_time_update_from_pot(void)
{
    if (menu_editor_update_from_pot(&time_editor, g_pot_value, menu_get_cursor()))
    {
        g_force_redraw = true;
    }
}

void menu_set_time_custom_draw(void)
{
    menu_editor_draw(&time_editor, menu_get_cursor(), &g_force_redraw);
}