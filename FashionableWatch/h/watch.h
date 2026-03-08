#ifndef WATCH_H
#define WATCH_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
} WatchTime;

typedef enum
{
    STATE_CLOCK,
    STATE_MENU,
    STATE_ALARM_RINGING
} AppState;

typedef enum
{
    FACE_DIGI,
    FACE_ANALOG
} FaceType;

typedef enum
{
    TIME_12H,
    TIME_24H
} TimeFormat;

extern uint8_t analogTheme;
extern volatile uint16_t g_pot_value;

/* shared globals from time_engine.c */
extern volatile AppState myState;
extern volatile FaceType myFace;
extern volatile TimeFormat myFormat;
extern volatile WatchTime now;
extern volatile bool g_force_redraw;
extern volatile bool g_tick_1s;
extern volatile uint32_t ms_ticks;

/* board */
void init_all(void);

/* potentiometer */
void pot_update(void);

/* accelerometer */
uint8_t accel_init(void);
uint8_t accel_is_face_down(void);
void accel_debug_display(void);

#endif