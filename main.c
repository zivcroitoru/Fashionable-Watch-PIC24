#include "FashionableWatch/h/watch.h"
#include "System/system.h"

int main(void)
{
    init_all();

    while (1)
    {
        check_inputs();
        check_gestures();

        if (g_force_redraw)
        {
            update_display();
            g_force_redraw = false;
        }

        if (g_tick_1s)
        {
            g_tick_1s = false;

            update_display();
            check_alarm_timeout();
        }
    }

    return 0;
}