#include "h/menu_pages.h"
#include "h/menu.h"
#include "h/watch.h"
#include "h/menu_editor.h"
#include <stdbool.h>
#include <stdint.h>

extern volatile WatchTime now;
extern volatile uint16_t g_pot_value;
extern volatile bool g_force_redraw;

static MenuEditField date_fields[] =
{
    {
        "Month", FIELD_TYPE_RANGE, (uint8_t*)&now.month, 0,
        1, 12, 30,
        12, 35, 40, 57,
        14, 40, 1, 1
    },
    {
        "Day", FIELD_TYPE_RANGE, (uint8_t*)&now.day, 0,
        1, 31, 15,
        50, 35, 80, 57,
        55, 40, 1, 1
    }
};

static MenuEditorState date_editor =
{
    date_fields,
    2,
    "BACK",
    25, 80, 70, 95,
    35, 84,
    "",
    0, -1, 0, 0, 0, "",
    true, -1
};

uint8_t menu_set_date_get_count(void)
{
    return 3; /* 2 fields + back */
}

const char* menu_set_date_get_item(uint8_t index)
{
    (void)index;
    return "";
}

void menu_set_date_reset_state(void)
{
    menu_editor_reset(&date_editor);
}

void menu_set_date_on_select(uint8_t index)
{
    bool go_back = false;

    menu_editor_on_select(&date_editor, index, &go_back);

    if (go_back) {
        menu_editor_reset(&date_editor);
        menu_set_current_page(MENU_PAGE_MAIN);
        return;
    }

    menu_set_cursor((index + 1) % menu_set_date_get_count());
    g_force_redraw = true;
}

void menu_set_date_update_from_pot(void)
{
    if (menu_editor_update_from_pot(&date_editor, g_pot_value, menu_get_cursor()))
    {
        g_force_redraw = true;
    }
}

void menu_set_date_custom_draw(void)
{
    menu_editor_draw(&date_editor, menu_get_cursor(), &g_force_redraw);
}