#ifndef WATCH_H
#define WATCH_H

#include <stdint.h>
#include <stdbool.h>
#include "../../oledDriver/oledC.h"
#include "../../oledDriver/oledC_colors.h"
#include "../../oledDriver/oledC_shapes.h"
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

extern volatile AppState myState;
extern volatile FaceType myFace;
extern volatile TimeFormat myFormat;
extern volatile WatchTime now;

extern volatile bool g_force_redraw;
extern volatile bool g_tick_1s;
extern volatile uint32_t ms_ticks;

extern uint8_t analogTheme;

void init_all(void);
void pot_update(void);
void check_inputs(void);
void check_gestures(void);
void update_display(void);
void update_analog_face(void);
uint8_t accel_is_face_down(void);
void accel_debug_display(void);

#endif