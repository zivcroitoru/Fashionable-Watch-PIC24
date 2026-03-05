#include "FashionableWatch/h/watch.h"
#include <xc.h>

int main(void) {
    init_all(); 
    __builtin_enable_interrupts(); // ADD THIS LINE

    while(1) {
        check_inputs();         //
        check_gestures();       
        update_display();       //
        check_alarm_timeout();  //

        // ADD THIS: Wait 50-100ms so the screen isn't overwhelmed
    }
    return 0;
}