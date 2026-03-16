#include "h/menu_pages.h"
#include "h/menu.h"
#include "h/watch.h"

#include <stdio.h>

uint8_t menu_format_get_count(void)
{
    return 3;
}

const char* menu_format_get_item(uint8_t index)
{
    static char row1[20];
    static char row2[20];

    switch (index)
    {
        case 0:
            return "< Back";

        case 1:
            sprintf(row1, "12H [%c]", (myFormat == TIME_12H) ? 'X' : ' ');
            return row1;

        case 2:
            sprintf(row2, "24H [%c]", (myFormat == TIME_24H) ? 'X' : ' ');
            return row2;

        default:
            return "";
    }
}

void menu_format_on_select(uint8_t index)
{
    switch (index)
    {
        case 0:
            menu_set_current_page(MENU_PAGE_MAIN);
            break;

        case 1:
            myFormat = TIME_12H;
            g_force_redraw = true;
            break;

        case 2:
            myFormat = TIME_24H;
            g_force_redraw = true;
            break;

        default:
            break;
    }
}