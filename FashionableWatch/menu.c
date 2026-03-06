#include "h/menu.h"
#include "../oledDriver/oledC.h"
#include "../oledDriver/oledC_shapes.h"
#include <stdbool.h>
#include <stdint.h>

// --- 1. VARIABLE DEFINITIONS ---
int8_t menu_cursor_index = 0;
WatchState current_state = STATE_MAIN_MENU;

// Menu text array
const char* main_menu_items[5] = {
    "Display Mode",
    "12H/24H Interval",
    "Set Time",
    "Set Date",
    "Alarm"
};

// --- 2. INTERNAL UI CONSTANTS ---
#define MENU_X          0
#define MENU_Y_START    25
#define ITEM_HEIGHT     12
#define MENU_WIDTH      95
#define MENU_ITEM_COUNT 5

// Keeps track of what row was last highlighted on screen
static int8_t last_drawn_cursor = -1;

// Draw one menu row only
static void menu_draw_row(uint8_t index, bool selected)
{
    uint16_t bg_color       = 0x0000;
    uint16_t text_color     = 0xFFFF;
    uint16_t highlight_bg   = 0xFFFF;
    uint16_t highlight_text = 0x0000;

    uint8_t y_pos = MENU_Y_START + (index * ITEM_HEIGHT);

    // Clear only this row
    oledC_DrawRectangle(MENU_X,
                        y_pos,
                        MENU_X + MENU_WIDTH,
                        y_pos + (ITEM_HEIGHT - 1),
                        selected ? highlight_bg : bg_color);

    // Draw row text
    oledC_DrawString(MENU_X + 2,
                     y_pos + 2,
                     1,
                     1,
                     (uint8_t*)main_menu_items[index],
                     selected ? highlight_text : text_color);
}

// Call this when entering the menu or after a full-screen clear
void menu_reset_draw_cache(void)
{
    last_drawn_cursor = -1;
}

void menu_draw_main_list(void)
{
    // First draw after entering menu: draw all rows
    if (last_drawn_cursor < 0 || last_drawn_cursor >= MENU_ITEM_COUNT)
    {
        for (uint8_t i = 0; i < MENU_ITEM_COUNT; i++)
        {
            menu_draw_row(i, (i == menu_cursor_index));
        }
    }
    // Cursor moved: redraw only old row + new row
    else if (last_drawn_cursor != menu_cursor_index)
    {
        menu_draw_row((uint8_t)last_drawn_cursor, false);
        menu_draw_row((uint8_t)menu_cursor_index, true);
    }
    // Same cursor, but forced redraw
    else
    {
        menu_draw_row((uint8_t)menu_cursor_index, true);
    }

    last_drawn_cursor = menu_cursor_index;
}

void menu_draw(void)
{
    menu_draw_main_list();
}