#include "../h/watch.h"
#include <xc.h>
#include <stdio.h>
#include <stdbool.h>
#include "../../System/system.h"
#include "../../oledDriver/oledC.h"
#include "../../oledDriver/oledC_shapes.h"
#include "../../System/delay.h"

// 1. Global Variables & State
AppState myState = STATE_CLOCK;
FaceType myFace = FACE_DIGI;
TimeFormat myFormat = TIME_12H; // User can choose 12H or 24H 
WatchTime now = {0, 0, 12, 1, 1}; 
WatchTime alarmTime = {0, 0, 7, 1, 1}; // Default alarm [cite: 45]
bool alarmActive = false; // Is the alarm currently ringing? [cite: 79]
bool alarmEnabled = true; // Is the alarm set to ON? [cite: 14, 47]

static WatchTime last_drawn = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// 2. Timer 1 ISR - Updates time every 1 second [cite: 42, 115]
void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void) {
    _T1IF = 0; 
    
    now.sec++;
    if (now.sec >= 60) {
        now.sec = 0;
        now.min++;
        if (now.min >= 60) {
            now.min = 0;
            now.hour++;
            if (now.hour >= 24) {
                now.hour = 0;
                now.day++; // Logic for specific month lengths needed for final [cite: 75]
            }
        }
    }
}

// 3. Helper: AM/PM and Hour Conversion 
bool is_pm(uint8_t hour) {
    return (hour >= 12);
}

uint8_t get_display_hour(uint8_t hour) {
    if (myFormat == TIME_24H) return hour;
    uint8_t h = hour % 12;
    return (h == 0) ? 12 : h;
}

// 4. Update Functions
void update_display(void) {
    char buf[20];
    
    // A. ALARM INDICATOR 
    // Draw a small icon in the top corner if alarm is armed
    if (alarmEnabled) {
        oledC_DrawCircle(10, 10, 3, 0xFFFF); // Placeholder for alarm icon [cite: 19]
    } else {
        oledC_DrawCircle(10, 10, 3, 0x0000); 
    }

    // B. HOURS & MINUTES
    if (now.hour != last_drawn.hour || now.min != last_drawn.min) {
        // Erase old
        sprintf(buf, "%02d:%02d", get_display_hour(last_drawn.hour), last_drawn.min);
        oledC_DrawString(10, 35, 2, 2, (uint8_t*)buf, 0x0000); 

        // Draw new [cite: 16]
        sprintf(buf, "%02d:%02d", get_display_hour(now.hour), now.min);
        oledC_DrawString(10, 35, 2, 2, (uint8_t*)buf, 0xFFFF);
    }

    // C. SECONDS
    if (now.sec != last_drawn.sec) {
        sprintf(buf, ":%02d", last_drawn.sec);
        oledC_DrawString(70, 42, 1, 1, (uint8_t*)buf, 0x0000); 
        sprintf(buf, ":%02d", now.sec);
        oledC_DrawString(70, 42, 1, 1, (uint8_t*)buf, 0xFFFF);
    }

    // D. AM/PM INDICATOR 
    if (myFormat == TIME_12H || myFace == FACE_ANALOG) {
        if (is_pm(now.hour) != is_pm(last_drawn.hour) || last_drawn.hour == 0xFF) {
            oledC_DrawString(2, 85, 1, 1, (uint8_t*)(is_pm(last_drawn.hour) ? "pm" : "am"), 0x0000);
            oledC_DrawString(2, 85, 1, 1, (uint8_t*)(is_pm(now.hour) ? "pm" : "am"), 0xFFFF);
        }
    }

    // E. DATE [cite: 13, 18]
    if (now.day != last_drawn.day || now.month != last_drawn.month) {
        sprintf(buf, "%02d/%02d", last_drawn.day, last_drawn.month);
        oledC_DrawString(65, 85, 1, 1, (uint8_t*)buf, 0x0000); 
        sprintf(buf, "%02d/%02d", now.day, now.month);
        oledC_DrawString(65, 85, 1, 1, (uint8_t*)buf, 0xFFFF);
    }

    last_drawn = now;
}

void init_all(void) {
    SYSTEM_Initialize();
    oledC_setup();
    
    // Timer 1 Setup for 1 second [cite: 115]
    T1CON = 0x8030; 
    PR1 = 15625; 
    _T1IF = 0; 
    _T1IE = 1; 
    
    oledC_DrawRectangle(0, 0, 95, 95, 0x0000); 
}
// Add these to the end of watch.c
void check_inputs(void) {
    // This will handle S1/S2 and Potentiometer later
}

void check_gestures(void) {
    // This will handle the Accelerometer (shaking/flipping) later
}

void check_alarm_timeout(void) {
    // This will handle the 20-second alarm cutoff later
}