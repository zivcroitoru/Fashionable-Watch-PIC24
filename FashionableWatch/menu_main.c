
#include "h/menu.h"
#include "h/menu_pages.h"

uint8_t menu_main_get_count(void)
{
    return 5;
}

const char* menu_main_get_item(uint8_t index)
{
    switch (index)
    {
        case 0: return "Display";
        case 1: return "Time Format";
        case 2: return "Set Time";
        case 3: return "Set Date";
        case 4: return "Alarm";
        default: return "";
    }
}
void menu_main_on_select(uint8_t index)
{
    switch (index)
    {
        case 0:
            menu_set_current_page(MENU_PAGE_DISPLAY);
            break;

        case 1:
            menu_set_current_page(MENU_PAGE_FORMAT);
            break;

        case 2:
            menu_set_current_page(MENU_PAGE_SET_TIME);
            menu_set_cursor(0);              // start on hours
            menu_set_time_reset_state();     // reset edit state
            break;

        case 3:
            menu_set_current_page(MENU_PAGE_SET_DATE);
            break;

        case 4:
            menu_set_current_page(MENU_PAGE_ALARM);
            break;

        default:
            break;
    }
}
