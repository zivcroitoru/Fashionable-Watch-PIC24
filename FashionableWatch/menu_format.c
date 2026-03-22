#include "h/menu_pages.h"
#include "h/menu.h"
#include "h/watch.h"

#include <stdio.h>

// Number of items in this menu
uint8_t menu_format_get_count(void)
{
    return 3;
}

// Returns text for each menu row
const char* menu_format_get_item(uint8_t index)
{
    static char row1[20];
    static char row2[20];

    switch (index)
    {
        case 0:
            return "< Back"; // go back to main menu

        case 1:
            // 12-hour format + mark if selected
            sprintf(row1, "12H [%c]", (myFormat == TIME_12H) ? 'X' : ' ');
            return row1;

        case 2:
            // 24-hour format + mark if selected
            sprintf(row2, "24H [%c]", (myFormat == TIME_24H) ? 'X' : ' ');
            return row2;

        default:
            return ""; // safety fallback
    }
}

// What happens when user presses confirm
void menu_format_on_select(uint8_t index)
{
    switch (index)
    {
        case 0:
            // go back to main menu
            menu_set_current_page(MENU_PAGE_MAIN);
            break;

        case 1:
            // switch to 12-hour format
            myFormat = TIME_12H;
            g_force_redraw = true; // refresh display
            break;

        case 2:
            // switch to 24-hour format
            myFormat = TIME_24H;
            g_force_redraw = true;
            break;

        default:
            break;
    }
}