#include "FashionableWatch/h/watch.h"
#include "System/system.h"
#include "FashionableWatch/h/alarm.h"

int main(void)
{
    init_all(); //init all hardware (clock, OLED, inputs)

    while (1)
    {
        pot_update(); //read potentiometer
        check_inputs(); // handle button presses
        check_gestures(); // handle accelerometer gestures
        
        // runs every sec, triggered by timer interrupt
        if (g_tick_1s)
        {
            g_tick_1s = false; // reset the flag
            alarm_update_1s(); // update alarm logic
            g_force_redraw = true; // request screen refresh
        }
        // redraw screen only when needed to avoid flicker
        if (g_force_redraw)
        {
            update_display(); 
            g_force_redraw = false; // reset redraw request
        }
    }
    return 0;
}