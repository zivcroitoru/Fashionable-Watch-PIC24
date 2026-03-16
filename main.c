#include "FashionableWatch/h/watch.h"
#include "System/system.h"
#include "FashionableWatch/h/alarm.h"

int main(void)
{
    init_all();

    while (1)
    {
        pot_update();
        check_inputs();
        check_gestures();

        if (g_tick_1s)
        {
            g_tick_1s = false;
            alarm_update_1s();
            g_force_redraw = true;
        }

        if (g_force_redraw)
        {
            update_display();
            g_force_redraw = false;
        }
    }

    return 0;
}