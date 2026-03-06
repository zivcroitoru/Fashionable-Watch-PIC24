#include "h/menu_pages.h"
#include "h/menu.h"
#include "h/watch.h"


#include <stdio.h>
#include <stdbool.h>

extern WatchTime alarmTime;
extern bool alarmEnabled;

uint8_t menu_alarm_get_count(void)
{
    return 4;
}

const char* menu_alarm_get_item(uint8_t index)
{
    static char row1[24];
    static char row2[24];
    static char row3[24];

    switch (index)
    {
        case 0:
            return "< Back";

        case 1:
            sprintf(row1, "Enabled [%c]", alarmEnabled ? 'X' : ' ');
            return row1;

        case 2:
            sprintf(row2, "Hour +   (%02u)", alarmTime.hour);
            return row2;

        case 3:
            sprintf(row3, "Minute + (%02u)", alarmTime.min);
            return row3;

        default:
            return "";
    }
}

void menu_alarm_on_select(uint8_t index)
{
    switch (index)
    {
        case 0:
            menu_set_current_page(MENU_PAGE_MAIN);
            break;

        case 1:
            alarmEnabled = !alarmEnabled;
            g_force_redraw = true;
            break;

        case 2:
            alarmTime.hour++;
            if (alarmTime.hour >= 24)
                alarmTime.hour = 0;
            g_force_redraw = true;
            break;

        case 3:
            alarmTime.min++;
            if (alarmTime.min >= 60)
                alarmTime.min = 0;
            g_force_redraw = true;
            break;

        default:
            break;
    }
}