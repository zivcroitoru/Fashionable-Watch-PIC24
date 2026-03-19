#ifndef MENU_PAGES_H
#define MENU_PAGES_H

#include <stdint.h>
#include <stdbool.h>
// Main
uint8_t menu_main_get_count(void);
const char* menu_main_get_item(uint8_t index);
void menu_main_on_select(uint8_t index);

// Display
uint8_t menu_display_get_count(void);
const char* menu_display_get_item(uint8_t index);
void menu_display_on_select(uint8_t index);

// Analog Theme
uint8_t menu_analog_theme_get_count(void);
const char* menu_analog_theme_get_item(uint8_t index);
void menu_analog_theme_on_select(uint8_t index);

// Format
uint8_t menu_format_get_count(void);
const char* menu_format_get_item(uint8_t index);
void menu_format_on_select(uint8_t index);

// Set Time
uint8_t menu_set_time_get_count(void);
const char* menu_set_time_get_item(uint8_t index);
void menu_set_time_on_select(uint8_t index);
bool menu_set_time_update_from_pot(void);

// Set Date
uint8_t menu_set_date_get_count(void);
const char* menu_set_date_get_item(uint8_t index);
void menu_set_date_on_select(uint8_t index);

// Alarm
uint8_t menu_alarm_get_count(void);
const char* menu_alarm_get_item(uint8_t index);
void menu_alarm_on_select(uint8_t index);

#endif