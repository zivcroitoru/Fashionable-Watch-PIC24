#include "h/menu_pages.h"
#include "h/menu.h"
#include "h/watch.h"
#include "h/menu_editor.h"
#include <stdbool.h>
#include <stdint.h>

// Real clock values from the system
extern volatile WatchTime now;
extern volatile uint16_t g_pot_value;
extern volatile bool g_force_redraw;

// -----------------------------------------------------------------------------
// Temporary editable copy
// -----------------------------------------------------------------------------
static uint8_t edit_hour = 0;
static uint8_t edit_min  = 0;
static uint8_t edit_sec  = 0;

// -----------------------------------------------------------------------------
// Editable fields
// -----------------------------------------------------------------------------
static MenuEditField time_fields[] =
{
    {
        "Hour", FIELD_TYPE_RANGE, &edit_hour, 0,
        0, 23, 40,
        8, 33, 35, 57,
        10, 35, 2, 2
    },
    {
        "Min", FIELD_TYPE_RANGE, &edit_min, 0,
        0, 59, 15,
        38, 33, 65, 57,
        40, 35, 2, 2
    },
    {
        "Sec", FIELD_TYPE_RANGE, &edit_sec, 0,
        0, 59, 15,
        68, 40, 95, 56,
        70, 42, 1, 1
    }
};

// -----------------------------------------------------------------------------
// Editor layout/state
// -----------------------------------------------------------------------------
static MenuEditorState time_editor =
{
    time_fields,
    3,                  // Hour, Min, Sec
    "BACK",
    68, 80, 95, 95,
    70, 84,
    "",

    // Internal state
    0, -1, 0, 0, 0, "",
    true, -1
};

uint8_t menu_set_time_get_count(void)
{
    return 4; // 3 fields + BACK
}

const char* menu_set_time_get_item(uint8_t index)
{
    (void)index;
    return "";
}

// Called when entering the page
void menu_set_time_reset_state(void)
{
    // Copy real clock into temp editable values
    edit_hour = now.hour;
    edit_min  = now.min;
    edit_sec  = now.sec;

    menu_editor_reset(&time_editor);
}

// Save only if something changed
static bool menu_set_time_apply_changes(void)
{
    bool changed = false;

    if (now.hour != edit_hour) {
        now.hour = edit_hour;
        changed = true;
    }

    if (now.min != edit_min) {
        now.min = edit_min;
        changed = true;
    }

    if (now.sec != edit_sec) {
        now.sec = edit_sec;
        changed = true;
    }

    return changed;
}

/*
 * Live preview getter for the top bar.
 */
void menu_set_time_get_preview(WatchTime* t)
{
    if (!t) return;

    *t = now;          // keep date/month/year/weekday/etc
    t->hour = edit_hour;
    t->min  = edit_min;
    t->sec  = edit_sec;
}

void menu_set_time_on_select(uint8_t index)
{
    bool go_back = false;

    // BACK selected
    if (index == time_editor.field_count) {
        menu_set_time_apply_changes();
        menu_editor_reset(&time_editor);
        menu_set_current_page(MENU_PAGE_MAIN);
        g_force_redraw = true;
        return;
    }

    // Normal field select
    menu_editor_on_select(&time_editor, index, &go_back);

    if (go_back) {
        menu_set_time_apply_changes();
        menu_editor_reset(&time_editor);
        menu_set_current_page(MENU_PAGE_MAIN);
        g_force_redraw = true;
        return;
    }

    // Move to next item
    menu_set_cursor((index + 1) % menu_set_time_get_count());
    g_force_redraw = true;
}

bool menu_set_time_update_from_pot(void)
{
    bool changed = menu_editor_update_from_pot(&time_editor, g_pot_value, menu_get_cursor());

    if (changed) {
        g_force_redraw = true;
    }

    return changed;
}

void menu_set_time_custom_draw(void)
{
    menu_editor_draw(&time_editor, menu_get_cursor(), &g_force_redraw);
}