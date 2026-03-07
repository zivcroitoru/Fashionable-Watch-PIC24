#include "h/menu_pages.h"
#include "h/menu.h"
#include "h/watch.h"

#include <stdio.h>

uint8_t menu_display_get_count(void)
{
    return 3;
}

const char* menu_display_get_item(uint8_t index)
{
    static char row1[20];
    static char row2[20];

    switch (index)
    {
        case 0:
            return "< Back";

        case 1:
            sprintf(row1, "Digital [%c]", (myFace == FACE_DIGI) ? 'X' : ' ');
            return row1;

        case 2:
            return "Analog >";

        default:
            return "";
    }
}

void menu_display_on_select(uint8_t index)
{
    switch (index)
    {
        case 0:
            menu_set_current_page(MENU_PAGE_MAIN);
            break;

        case 1:
            myFace = FACE_DIGI;
            g_force_redraw = true;
            break;

        case 2:
            menu_set_current_page(MENU_PAGE_ANALOG_THEME);
            break;

        default:
            break;
    }
}