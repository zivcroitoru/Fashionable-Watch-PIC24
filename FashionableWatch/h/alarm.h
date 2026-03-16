#ifndef ALARM_H
#define ALARM_H

#include <stdbool.h>
#include <stdint.h>

void alarm_update_1s(void);
void alarm_draw_if_active(void);
bool alarm_is_ringing(void);
void alarm_stop(void);

#endif