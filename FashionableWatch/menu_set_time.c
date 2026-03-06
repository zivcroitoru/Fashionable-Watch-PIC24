#include "h/menu_pages.h"
#include "h/menu.h"
#include "h/watch.h"

#include <stdio.h>

extern volatile WatchTime now;

uint8_t menu_set_time_get_count(void)
{
    return 4;
}

const char* menu_set_time_get_item(uint8_t index)
{
    static char row1[24];
    static char row2[24];

    switch (index)
    {
        case 0:
            return "< Back";

        case 1:
            sprintf(row1, "Hour +   (%02u)", now.hour);
            return row1;

        case 2:
            sprintf(row2, "Minute + (%02u)", now.min);
            return row2;

        case 3:
            return "Reset Seconds";

        default:
            return "";
    }
}

void menu_set_time_on_select(uint8_t index)
{
    switch (index)
    {
        case 0:
            menu_set_current_page(MENU_PAGE_MAIN);
            break;

        case 1:
            now.hour++;
            if (now.hour >= 24)
                now.hour = 0;
            g_force_redraw = true;
            break;

        case 2:
            now.min++;
            if (now.min >= 60)
                now.min = 0;
            g_force_redraw = true;
            break;

        case 3:
            now.sec = 0;
            g_force_redraw = true;
            break;

        default:
            break;
    }
}