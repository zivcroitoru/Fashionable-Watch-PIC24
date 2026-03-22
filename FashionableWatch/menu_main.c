#include "h/menu.h"
#include "h/menu_pages.h"

// Number of items in the main menu
uint8_t menu_main_get_count(void)
{
    return 5;
}

// Returns the text for each menu item
const char* menu_main_get_item(uint8_t index)
{
    switch (index)
    {
        case 0: return "Display";       // change watch face
        case 1: return "Time Format";   // 12H / 24H
        case 2: return "Set Time";      // edit current time
        case 3: return "Set Date";      // edit date
        case 4: return "Alarm";         // alarm settings
        default: return "";             // safety fallback
    }
}

// What happens when user selects an item
void menu_main_on_select(uint8_t index)
{
    switch (index)
    {
        case 0:
            // go to display settings
            menu_set_current_page(MENU_PAGE_DISPLAY);
            break;

        case 1:
            // go to time format menu
            menu_set_current_page(MENU_PAGE_FORMAT);
            break;

        case 2:
            // go to set time menu
            menu_set_current_page(MENU_PAGE_SET_TIME);
            break;

        case 3:
            // go to set date menu
            menu_set_current_page(MENU_PAGE_SET_DATE);
            break;

        case 4:
            // go to alarm menu
            menu_set_current_page(MENU_PAGE_ALARM);
            break;

        default:
            break;
    }
}