#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <stdbool.h>

// This tracks what screen the user is looking at
typedef enum {
    STATE_WATCH_FACE,       // Not in menu, showing normal clock
    STATE_MAIN_MENU,        // Looking at the list of 5 options
    STATE_SET_DISPLAY_MODE,
    STATE_SET_12_24,
    STATE_SET_TIME_HR,      // Selecting the hour to change (like the bottom image)
    STATE_SET_TIME_MIN,     // Selecting the minute to change
    STATE_SET_TIME_SEC,
    STATE_SET_DATE_DAY,
    STATE_SET_DATE_MONTH,
    STATE_SET_ALARM_HR,
    STATE_SET_ALARM_MIN
} WatchState;

extern WatchState current_state;
extern int8_t menu_cursor_index; 

void menu_init(void);
void menu_draw(void);
void menu_handle_button_up(void);
void menu_handle_button_down(void);
void menu_handle_button_select(void);

#endif