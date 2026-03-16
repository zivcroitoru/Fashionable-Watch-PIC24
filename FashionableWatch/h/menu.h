#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include "menu_types.h"

void menu_enter(void);
void menu_exit(void);

void menu_next_item(void);
void menu_select_current(void);

void menu_draw(void);
void menu_reset_draw_cache(void);

MenuPage menu_get_current_page(void);
void menu_set_current_page(MenuPage page);

int8_t menu_get_cursor(void);
void menu_set_cursor(int8_t index);

const char* menu_get_title(void);

#endif