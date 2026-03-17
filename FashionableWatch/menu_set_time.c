#include "h/menu_pages.h"
#include "h/menu.h"
#include "h/watch.h"
#include "h/menu_editor.h"
#include <stdbool.h>
#include <stdint.h>

// External hardware/clock variables
extern volatile WatchTime now;      // The actual clock time
extern volatile uint16_t g_pot_value; // Current value of the dial (potentiometer)
extern volatile bool g_force_redraw;  // UI refresh flag

// Define the three editable fields for the "Set Time" screen
static MenuEditField time_fields[] =
{
    {
        "Hour", FIELD_TYPE_RANGE, (uint8_t*)&now.hour, 0,
        0, 23, 40,      // Min 0, Max 23
        8, 33, 35, 57,  // Visual coordinates for the box/text
        10, 35, 2, 2    // Font size and padding
    },
    {
        "Min", FIELD_TYPE_RANGE, (uint8_t*)&now.min, 0,
        0, 59, 15,      // Min 0, Max 59
        38, 33, 65, 57,
        40, 35, 2, 2
    },
    {
        "Sec", FIELD_TYPE_RANGE, (uint8_t*)&now.sec, 0,
        0, 59, 15,      // Min 0, Max 59
        68, 40, 95, 56,
        70, 42, 1, 1
    }
};

// Configuration for the Editor UI (Layout and Field mapping)
static MenuEditorState time_editor =
{
    time_fields,
    3,                  // Number of fields (H, M, S)
    "BACK",             // Label for the exit button
    25, 80, 70, 95,     // Coordinates for the Back button
    35, 84,             // Text position for Back button
    "SET TIME",         // Screen title
    0, -1, 0, 0, 0, "", // Internal editor state (don't touch)
    true, -1
};

// Returns total items (3 fields + 1 Back button)
uint8_t menu_set_time_get_count(void)
{
    return 4; 
}

// Unused here because this page uses a custom editor draw instead of a list
const char* menu_set_time_get_item(uint8_t index)
{
    (void)index;
    return "";
}

// Resets the highlight/selection when entering the screen
void menu_set_time_reset_state(void)
{
    menu_editor_reset(&time_editor);
}

// Handles button clicks: cycles between fields or exits if "Back" is selected
void menu_set_time_on_select(uint8_t index)
{
    bool go_back = false;

    // Process selection logic in the editor
    menu_editor_on_select(&time_editor, index, &go_back);

    if (go_back) {
        menu_editor_reset(&time_editor);
        menu_set_current_page(MENU_PAGE_MAIN); // Return to main menu
        return;
    }

    // Move cursor to the next field automatically after clicking
    menu_set_cursor((index + 1) % menu_set_time_get_count());
    g_force_redraw = true;
}

// Reads the hardware dial (potentiometer) to change the values of H, M, or S
void menu_set_time_update_from_pot(void)
{
    if (menu_editor_update_from_pot(&time_editor, g_pot_value, menu_get_cursor()))
    {
        g_force_redraw = true;
    }
}

// Draws the graphical editor screen to the OLED
void menu_set_time_custom_draw(void)
{
    menu_editor_draw(&time_editor, menu_get_cursor(), &g_force_redraw);
}