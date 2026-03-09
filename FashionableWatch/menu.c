#include "h/menu.h"
#include "h/menu_pages.h"
#include "h/watch.h"

#include "../oledDriver/oledC.h"
#include "../oledDriver/oledC_shapes.h"

#include <stdbool.h>
#include <stdint.h>

#define MENU_X              0
#define MENU_Y_START        28
#define ITEM_HEIGHT         12
#define MENU_WIDTH          95

#define MENU_BODY_TOP       24
#define MENU_BODY_BOTTOM    95

static MenuPage current_page = MENU_PAGE_MAIN;
static int8_t menu_cursor_index = 0;

static int8_t last_drawn_cursor = -1;
static MenuPage last_drawn_page = (MenuPage)255;

static uint8_t menu_get_count_for_page(MenuPage page)
{
    switch (page)
    {
        case MENU_PAGE_MAIN:         return menu_main_get_count();
        case MENU_PAGE_DISPLAY:      return menu_display_get_count();
        case MENU_PAGE_ANALOG_THEME: return menu_analog_theme_get_count();
        case MENU_PAGE_FORMAT:       return menu_format_get_count();
        case MENU_PAGE_SET_TIME:     return menu_set_time_get_count();
        case MENU_PAGE_SET_DATE:     return menu_set_date_get_count();
        case MENU_PAGE_ALARM:        return menu_alarm_get_count();
        default:                     return 0;
    }
}

static const char* menu_get_item_for_page(MenuPage page, uint8_t index)
{
    switch (page)
    {
        case MENU_PAGE_MAIN:         return menu_main_get_item(index);
        case MENU_PAGE_DISPLAY:      return menu_display_get_item(index);
        case MENU_PAGE_ANALOG_THEME: return menu_analog_theme_get_item(index);
        case MENU_PAGE_FORMAT:       return menu_format_get_item(index);
        case MENU_PAGE_SET_TIME:     return menu_set_time_get_item(index);
        case MENU_PAGE_SET_DATE:     return menu_set_date_get_item(index);
        case MENU_PAGE_ALARM:        return menu_alarm_get_item(index);
        default:                     return "";
    }
}

static void menu_select_for_page(MenuPage page, uint8_t index)
{
    switch (page)
    {
        case MENU_PAGE_MAIN:         menu_main_on_select(index);         break;
        case MENU_PAGE_DISPLAY:      menu_display_on_select(index);      break;
        case MENU_PAGE_ANALOG_THEME: menu_analog_theme_on_select(index); break;
        case MENU_PAGE_FORMAT:       menu_format_on_select(index);       break;
        case MENU_PAGE_SET_TIME:     menu_set_time_on_select(index);     break;
        case MENU_PAGE_SET_DATE:     menu_set_date_on_select(index);     break;
        case MENU_PAGE_ALARM:        menu_alarm_on_select(index);        break;
        default: break;
    }
}

static void menu_draw_row(uint8_t index, bool selected)
{
    uint16_t bg_color       = 0x0000;
    uint16_t text_color     = 0xFFFF;
    uint16_t highlight_bg   = 0xFFFF;
    uint16_t highlight_text = 0x0000;

    uint8_t y = MENU_Y_START + (index * ITEM_HEIGHT);
    const char* label = menu_get_item_for_page(current_page, index);

    if (selected) {
        oledC_DrawRectangle(MENU_X, y, MENU_X + MENU_WIDTH, y + (ITEM_HEIGHT - 1), highlight_bg);
        oledC_DrawString(MENU_X + 2, y + 2, 1, 1, (uint8_t*)label, highlight_text);
    } else {
        oledC_DrawRectangle(MENU_X, y, MENU_X + MENU_WIDTH, y + (ITEM_HEIGHT - 1), bg_color);
        oledC_DrawString(MENU_X + 2, y + 2, 1, 1, (uint8_t*)label, text_color);
    }
}

void menu_reset_draw_cache(void)
{
    last_drawn_cursor = -1;
    last_drawn_page = (MenuPage)255;
}

void menu_enter(void)
{
    myState = STATE_MENU;
    current_page = MENU_PAGE_MAIN;
    menu_cursor_index = 0;
    menu_reset_draw_cache();
    g_force_redraw = true;
}

void menu_exit(void)
{
    myState = STATE_CLOCK;
    current_page = MENU_PAGE_MAIN;
    menu_cursor_index = 0;
    menu_reset_draw_cache();
    g_force_redraw = true;
}

void menu_next_item(void)
{
    uint8_t count = menu_get_count_for_page(current_page);

    if (count == 0)
        return;

    menu_cursor_index++;
    if (menu_cursor_index >= count)
        menu_cursor_index = 0;

    g_force_redraw = true;
}

void menu_select_current(void)
{
    menu_select_for_page(current_page, (uint8_t)menu_cursor_index);
    g_force_redraw = true;
}

MenuPage menu_get_current_page(void)
{
    return current_page;
}

void menu_set_current_page(MenuPage page)
{
    current_page = page;

    switch (page)
    {
        case MENU_PAGE_SET_TIME:
            menu_set_time_reset_state();
            menu_cursor_index = 1;
            break;

        case MENU_PAGE_SET_DATE:
            menu_set_date_reset_state();
            menu_cursor_index = 0;
            break;

        case MENU_PAGE_ALARM:
            menu_alarm_reset_state();
            menu_cursor_index = 0;
            break;

        default:
            menu_cursor_index = 0;
            break;
    }

    menu_reset_draw_cache();
    g_force_redraw = true;
}

int8_t menu_get_cursor(void)
{
    return menu_cursor_index;
}

void menu_set_cursor(int8_t index)
{
    menu_cursor_index = index;
    g_force_redraw = true;
}

const char* menu_get_title(void)
{
    switch (current_page)
    {
        case MENU_PAGE_MAIN:         return "MENU";
        case MENU_PAGE_DISPLAY:      return "DISPLAY";
        case MENU_PAGE_ANALOG_THEME: return "ANALOG";
        case MENU_PAGE_FORMAT:       return "FORMAT";
        case MENU_PAGE_SET_TIME:     return "SET TIME";
        case MENU_PAGE_SET_DATE:     return "SET DATE";
        case MENU_PAGE_ALARM:        return "ALARM";
        default:                     return "MENU";
    }
}

void menu_draw(void)
{
    // Custom pages: content only
    if (current_page == MENU_PAGE_SET_TIME)
    {
        menu_set_time_custom_draw();
        return;
    }

    if (current_page == MENU_PAGE_SET_DATE)
    {
        menu_set_date_custom_draw();
        return;
    }

    if (current_page == MENU_PAGE_ALARM)
    {
        menu_alarm_custom_draw();
        return;
    }

    // Standard list pages: content only
    {
        bool page_changed = (current_page != last_drawn_page);
        uint8_t count = menu_get_count_for_page(current_page);
        uint8_t i;

        if (page_changed || last_drawn_cursor < 0)
        {
            oledC_DrawRectangle(0, MENU_BODY_TOP, 95, MENU_BODY_BOTTOM, 0x0000);

            for (i = 0; i < count; i++)
            {
                menu_draw_row(i, (i == (uint8_t)menu_cursor_index));
            }
        }
        else if (last_drawn_cursor != menu_cursor_index)
        {
            menu_draw_row((uint8_t)last_drawn_cursor, false);
            menu_draw_row((uint8_t)menu_cursor_index, true);
        }
        else
        {
            menu_draw_row((uint8_t)menu_cursor_index, true);
        }

        last_drawn_cursor = menu_cursor_index;
        last_drawn_page = current_page;
    }
}