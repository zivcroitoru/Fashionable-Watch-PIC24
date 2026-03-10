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

    // Check if we are actually using the Analog face
    bool is_analog = (myFace == FACE_ANALOG);

    switch (index)
    {
        case 0:
            return "< Back";

        case 1:
            sprintf(row1, "Theme 1 [%c]", (is_analog && analogTheme == 0) ? 'X' : ' ');
            return row1;

        case 2:
            sprintf(row2, "Theme 2 [%c]", (is_analog && analogTheme == 1) ? 'X' : ' ');
            return row2;

        case 3:
            sprintf(row3, "Theme 3 [%c]", (is_analog && analogTheme == 2) ? 'X' : ' ');
            return row3;

        default:
            return "";
    }
}

void menu_analog_theme_on_select(uint8_t index)
{
    if (index == 0) {
        menu_set_current_page(MENU_PAGE_DISPLAY);
        return;
    }

    if (index == 1) analogTheme = 0;
    else if (index == 2) analogTheme = 1;
    else if (index == 3) analogTheme = 2;

    myFace = FACE_ANALOG;    // Turns on Analog so the 'X' appears
    menu_reset_draw_cache(); // Clears old screen data
    g_force_redraw = true;   // Draws the new 'X'
}