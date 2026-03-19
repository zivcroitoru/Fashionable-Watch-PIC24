#include "h/menu_pages.h"
#include "h/menu.h"
#include "h/watch.h"
#include "h/menu_editor.h"
#include "../oledDriver/oledC.h"
#include <stdbool.h>
#include <stdint.h>

extern volatile WatchTime now;
extern volatile uint16_t g_pot_value;
extern volatile bool g_force_redraw;

/*
 * Date fields in DD / MM order
 * Each field:
 * - pointer to actual value in 'now'
 * - range (day: 1?31, month: 1?12)
 * - box position
 * - text position + scale (2x for bigger numbers)
 */
static MenuEditField date_fields[] =
{
    {
        "Day", FIELD_TYPE_RANGE, (uint8_t*)&now.day, 0,
        1, 31, 15,
        12, 30, 40, 60,   // box
        14, 35, 2, 2      // big number
    },
    {
        "Month", FIELD_TYPE_RANGE, (uint8_t*)&now.month, 0,
        1, 12, 30,
        50, 30, 80, 60,
        52, 35, 2, 2
    }
};

/*
 * Editor state
 * Title is empty because top bar already shows context
 */
static MenuEditorState date_editor =
{
    date_fields,
    2,
    "BACK",
    68, 80, 95, 95,
    70, 84,
    "",
    0, -1, 0, 0, 0, "",
    true, -1
};

/* 2 fields + BACK */
uint8_t menu_set_date_get_count(void)
{
    return 3;
}

/* Not used (custom draw handles UI) */
const char* menu_set_date_get_item(uint8_t index)
{
    (void)index;
    return "";
}

/* Reset editor state when entering screen */
void menu_set_date_reset_state(void)
{
    menu_editor_reset(&date_editor);
}

/* Handle button select */
void menu_set_date_on_select(uint8_t index)
{
    bool go_back = false;

    menu_editor_on_select(&date_editor, index, &go_back);

    if (go_back)
    {
        menu_editor_reset(&date_editor);
        menu_set_current_page(MENU_PAGE_MAIN);
        return;
    }

    /* Move cursor to next item */
    menu_set_cursor((index + 1) % menu_set_date_get_count());

    g_force_redraw = true;
}

/* Update values from potentiometer */
void menu_set_date_update_from_pot(void)
{
    (void)menu_editor_update_from_pot(&date_editor, g_pot_value, menu_get_cursor());
}

/* Custom draw (numbers + labels) */
void menu_set_date_custom_draw(void)
{
    menu_editor_draw(&date_editor, menu_get_cursor(), &g_force_redraw);

    /* Labels under numbers */
    oledC_DrawString(22, 65, 1, 1, (uint8_t*)"D", 0xFFFF);
    oledC_DrawString(60, 65, 1, 1, (uint8_t*)"M", 0xFFFF);
}