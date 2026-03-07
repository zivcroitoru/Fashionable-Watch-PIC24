#include "h/menu_pages.h"
#include "h/menu_types.h"

#include "h/menu.h"
#include "h/watch.h"

#include <stdio.h>

uint8_t menu_analog_theme_get_count(void)
{
    return 4;
}

const char* menu_analog_theme_get_item(uint8_t index)
{
    static char row1[20];
    static char row2[20];
    static char row3[20];

    switch (index)
    {
        case 0:
            return "< Back";

        case 1:
            sprintf(row1, "Theme 1 [%c]", (analogTheme == 0) ? 'X' : ' ');
            return row1;

        case 2:
            sprintf(row2, "Theme 2 [%c]", (analogTheme == 1) ? 'X' : ' ');
            return row2;

        case 3:
            sprintf(row3, "Theme 3 [%c]", (analogTheme == 2) ? 'X' : ' ');
            return row3;

        default:
            return "";
    }
}

void menu_analog_theme_on_select(uint8_t index)
{
    switch (index)
    {
        case 0:
            menu_set_current_page(MENU_PAGE_DISPLAY);
            break;

        case 1:
            analogTheme = 0;
            myFace = FACE_ANALOG;
            g_force_redraw = true;
            break;

        case 2:
            analogTheme = 1;
            myFace = FACE_ANALOG;
            g_force_redraw = true;
            break;

        case 3:
            analogTheme = 2;
            myFace = FACE_ANALOG;
            g_force_redraw = true;
            break;
    }
}