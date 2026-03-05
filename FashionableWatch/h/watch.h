#ifndef WATCH_H
#define WATCH_H

#include <stdint.h>
#include <stdbool.h>

// ===== States and Types =====
typedef enum { STATE_CLOCK, STATE_MENU, STATE_ALARM_RINGING } AppState;
typedef enum { FACE_DIGI, FACE_ANALOG } FaceType;
typedef enum { TIME_12H, TIME_24H } TimeFormat;

typedef struct {
    uint8_t sec, min, hour, day, month;
} WatchTime;

// ===== Shared Variables =====
extern volatile AppState myState;
extern volatile FaceType myFace;
extern volatile TimeFormat myFormat;

extern volatile WatchTime now;
extern volatile uint32_t ms_ticks;

extern volatile bool g_tick_1s;
extern volatile bool g_force_redraw;

extern bool alarmEnabled;
extern WatchTime alarmTime;

// ===== Functions =====
void init_all(void);
void check_inputs(void);
void check_gestures(void);
void update_display(void);
void check_alarm_timeout(void);

uint8_t get_display_hour(uint8_t hour);

#endif