#include "h/menu.h"
#include "h/menu_pages.h"
#include "h/watch.h"

#include "../oledDriver/oledC.h"
#include "../oledDriver/oledC_shapes.h"

#include <stdbool.h>
#include <stdint.h>

// UI Layout Constants
#define MENU_X              0
#define MENU_Y_START        28
#define ITEM_HEIGHT         12
#define MENU_WIDTH          95

#define MENU_BODY_TOP       24
#define MENU_BODY_BOTTOM    95

// Navigation state
static MenuPage current_page = MENU_PAGE_MAIN;
static int8_t menu_cursor_index = 0;

// Rendering cache to prevent flickering by only redrawing what changed
static int8_t last_drawn_cursor = -1;
static MenuPage last_drawn_page = (MenuPage)255;

// Helper func: Returns how many items are in the current menu list
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

// Helper func: Returns the text string for a specific menu row
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

// Executes the action associated with a menu item when clicked
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

// Draws a single line of the menu (handles inverted colors for selected items)
static void menu_draw_row(uint8_t index, bool selected)
{
    uint16_t bg_color       = 0x0000; // Black
    uint16_t text_color     = 0xFFFF; // White
    uint16_t highlight_bg   = 0xFFFF; // White
    uint16_t highlight_text = 0x0000; // Black

    uint8_t y = MENU_Y_START + (index * ITEM_HEIGHT);
    const char* label = menu_get_item_for_page(current_page, index);

    if (selected) {
        // Draw highlighted bar with black text
        oledC_DrawRectangle(MENU_X, y, MENU_X + MENU_WIDTH, y + (ITEM_HEIGHT - 1), highlight_bg);
        oledC_DrawString(MENU_X + 2, y + 2, 1, 1, (uint8_t*)label, highlight_text);
    } else {
        // Draw black background with white text
        oledC_DrawRectangle(MENU_X, y, MENU_X + MENU_WIDTH, y + (ITEM_HEIGHT - 1), bg_color);
        oledC_DrawString(MENU_X + 2, y + 2, 1, 1, (uint8_t*)label, text_color);
    }
}

// Forces the renderer to redraw everything on the next cycle
void menu_reset_draw_cache(void)
{
    last_drawn_cursor = -1;
    last_drawn_page = (MenuPage)255;
}

// Transitions system from Clock to Menu mode
void menu_enter(void)
{
    myState = STATE_MENU;
    current_page = MENU_PAGE_MAIN;
    menu_cursor_index = 0;
    menu_reset_draw_cache();
    g_force_redraw = true;
}

// Transitions system back to Clock mode
void menu_exit(void)
{
    myState = STATE_CLOCK;
    current_page = MENU_PAGE_MAIN;
    menu_cursor_index = 0;
    menu_reset_draw_cache();
    g_force_redraw = true;
}

// Moves highlight to the next item (wraps around to top)
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

// Confirms the current selection
void menu_select_current(void)
{
    menu_select_for_page(current_page, (uint8_t)menu_cursor_index);
    
    // Wipe cache so dynamic text (like "ON/OFF") updates visually
    menu_reset_draw_cache(); 
    
    g_force_redraw = true;
}

MenuPage menu_get_current_page(void)
{
    return current_page;
}

// Changes menu page and resets scroll/drawing state
void menu_set_current_page(MenuPage page)
{
    current_page = page;

    // Reset sub-page states (specific cursor starts or values)
    switch (page)
    {
        case MENU_PAGE_SET_TIME:
            menu_set_time_reset_state();
            menu_cursor_index = 1; // Start on hours, not "Back"
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

// Returns the title string for the header bar
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

// Core drawing logic: optimized to only redraw changed pixels
void menu_draw(void)
{
    // 1. Check for pages with completely unique layouts
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

    // 2. Standard List Drawing (Main, Display, etc.)
    {
        bool page_changed = (current_page != last_drawn_page);
        uint8_t count = menu_get_count_for_page(current_page);
        uint8_t i;

        // If page changed, clear screen and draw all rows
        if (page_changed || last_drawn_cursor < 0)
        {
            oledC_DrawRectangle(0, MENU_BODY_TOP, 95, MENU_BODY_BOTTOM, 0x0000);

            for (i = 0; i < count; i++)
            {
                menu_draw_row(i, (i == (uint8_t)menu_cursor_index));
            }
        }
        // Optimization: Only redraw the previous row and the new row when moving cursor
        else if (last_drawn_cursor != menu_cursor_index)
        {
            menu_draw_row((uint8_t)last_drawn_cursor, false); // Un-highlight old
            menu_draw_row((uint8_t)menu_cursor_index, true);  // Highlight new
        }
        else
        {
            menu_draw_row((uint8_t)menu_cursor_index, true);
        }

        // Update cache
        last_drawn_cursor = menu_cursor_index;
        last_drawn_page = current_page;
    }
}