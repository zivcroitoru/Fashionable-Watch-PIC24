#include "h/menu_pages.h"
#include "h/menu.h"
#include "h/watch.h"

#include <stdio.h>

// How many items are in this menu
uint8_t menu_display_get_count(void)
{
    return 3;
}

// Returns the text for each menu row
const char* menu_display_get_item(uint8_t index)
{
    static char row[20]; // reusable buffer for dynamic text

    switch (index)
    {
        case 0: 
            return "< Back"; // go back to main menu

        case 1:
            // show Digital option + mark if currently selected
            sprintf(row, "Digital [%c]", (myFace == FACE_DIGI) ? 'X' : ' ');
            return row;

        case 2: 
            return "Analog >"; // go to analog submenu

        default: 
            return ""; // safety fallback
    }
}

// What happens when user presses "confirm" on an item
void menu_display_on_select(uint8_t index)
{
    switch (index)
    {
        case 0:
            // go back to main menu
            menu_set_current_page(MENU_PAGE_MAIN);
            break;

        case 1:
            // switch to digital face
            myFace = FACE_DIGI;
            g_force_redraw = true; // force screen update
            break;

        case 2:
            // open analog theme menu
            menu_set_current_page(MENU_PAGE_ANALOG_THEME);
            break;

        default:
            break;
    }
}