#ifndef WATCH_H
#define WATCH_H

#include <stdint.h>

// 1. States and Types
typedef enum { 
    STATE_CLOCK, 
    STATE_MENU, 
    STATE_ALARM_RINGING 
} AppState; 

typedef enum { 
    FACE_DIGI, 
    FACE_ANALOG // Fixed: Changed from FACE_ANLG to match watch.c 
} FaceType; 

typedef enum { 
    TIME_12H, 
    TIME_24H 
} TimeFormat; 

typedef struct {
    uint8_t sec, min, hour, day, month;
} WatchTime; 

// 2. Variable Declarations
extern AppState myState;
extern FaceType myFace;
extern TimeFormat myFormat;
extern WatchTime now;

// 3. Function Prototypes
void init_all(void);
void check_inputs(void);
void check_gestures(void);
void update_display(void);
void check_alarm_timeout(void);

#endif