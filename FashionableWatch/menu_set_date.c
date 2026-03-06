#include "h/menu_pages.h"
#include "h/menu.h"
#include "h/watch.h"

#include <stdio.h>

extern volatile WatchTime now;

static uint8_t local_get_days_in_month(uint8_t month)
{
    switch (month)
    {
        case 2:  return 28;
        case 4:
        case 6:
        case 9:
        case 11: return 30;
        default: return 31;
    }
}

uint8_t menu_set_date_get_count(void)
{
    return 3;
}

const char* menu_set_date_get_item(uint8_t index)
{
    static char row1[24];
    static char row2[24];

    switch (index)
    {
        case 0:
            return "< Back";

        case 1:
            sprintf(row1, "Day +   (%02u)", now.day);
            return row1;

        case 2:
            sprintf(row2, "Month + (%02u)", now.month);
            return row2;

        default:
            return "";
    }
}

void menu_set_date_on_select(uint8_t index)
{
    switch (index)
    {
        case 0:
            menu_set_current_page(MENU_PAGE_MAIN);
            break;

        case 1:
            now.day++;
            if (now.day > local_get_days_in_month(now.month))
                now.day = 1;
            g_force_redraw = true;
            break;

        case 2:
            now.month++;
            if (now.month > 12)
                now.month = 1;

            if (now.day > local_get_days_in_month(now.month))
                now.day = local_get_days_in_month(now.month);

            g_force_redraw = true;
            break;

        default:
            break;
    }
}