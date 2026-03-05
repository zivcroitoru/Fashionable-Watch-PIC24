#include "h/watch.h"
#include <stdio.h>
#include "../oledDriver/oledC_shapes.h"

static WatchTime last_drawn = {255,255,255,255,255};

void update_display(void)
{
    char buf[20];

    uint16_t bg = 0x0000;
    uint16_t text = 0xFFFF;

    WatchTime t = now;

    if (myState == STATE_MENU)
    {
        oledC_DrawRectangle(0,0,95,95,bg);

        oledC_DrawString(15,30,1,1,(uint8_t*)"MENU",text);

        sprintf(buf,"%02d:%02d:%02d",t.hour,t.min,t.sec);
        oledC_DrawString(15,50,1,1,(uint8_t*)buf,text);

        return;
    }

    if (myFace == FACE_ANALOG)
    {
        oledC_DrawRectangle(0,0,95,95,bg);

        oledC_DrawString(10,40,1,1,(uint8_t*)"ANALOG",text);

        sprintf(buf,"%02d:%02d:%02d",t.hour,t.min,t.sec);
        oledC_DrawString(10,60,1,1,(uint8_t*)buf,text);

        return;
    }

    bool hm_changed =
        (t.hour != last_drawn.hour) ||
        (t.min  != last_drawn.min);

    bool sec_changed = (t.sec != last_drawn.sec);

    if (hm_changed)
    {
        oledC_DrawRectangle(10,35,90,55,bg);

        sprintf(buf,"%02d:%02d",
                get_display_hour(t.hour),
                t.min);

        oledC_DrawString(10,35,2,2,(uint8_t*)buf,text);

        oledC_DrawRectangle(70,42,95,54,bg);

        sprintf(buf,":%02d",t.sec);
        oledC_DrawString(70,42,1,1,(uint8_t*)buf,text);
    }

    else if (sec_changed)
    {
        oledC_DrawRectangle(70,42,95,54,bg);

        sprintf(buf,":%02d",t.sec);
        oledC_DrawString(70,42,1,1,(uint8_t*)buf,text);
    }

    last_drawn = t;
}