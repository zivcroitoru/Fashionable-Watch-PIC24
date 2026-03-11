#include "h/watch.h"

#define CX 47
#define CY 47
#define CLOCK_R 20

const uint8_t edge_x[60] = {
    47, 52, 56, 61, 65, 70, 74, 78, 81, 85, 87, 90, 92, 93, 94, 94, 94, 93, 92, 90,
    87, 85, 81, 78, 74, 70, 65, 61, 56, 52, 47, 42, 38, 33, 29, 24, 20, 16, 13, 9,
    7, 4, 2, 1, 0, 0, 0, 1, 2, 4, 7, 9, 13, 16, 20, 24, 29, 33, 38, 42
};

const uint8_t edge_y[60] = {
    0, 0, 1, 2, 4, 7, 9, 13, 16, 20, 24, 29, 33, 38, 42, 47, 52, 56, 61, 65,
    70, 74, 78, 81, 85, 87, 90, 92, 93, 94, 94, 94, 93, 92, 90, 87, 85, 81, 78, 74,
    70, 65, 61, 56, 52, 47, 42, 38, 33, 29, 24, 20, 16, 13, 9, 7, 4, 2, 1, 0
};

static void get_scaled_point(uint8_t index, uint8_t length_percent, uint8_t *x, uint8_t *y)
{
    int dx = (int)edge_x[index] - CX;
    int dy = (int)edge_y[index] - CY;

    *x = (uint8_t)(CX + (dx * CLOCK_R * length_percent) / (47 * 100));
    *y = (uint8_t)(CY + (dy * CLOCK_R * length_percent) / (47 * 100));
}

static void draw_hand_thin(uint8_t index, uint8_t length_percent, uint16_t color)
{
    uint8_t ex, ey;
    get_scaled_point(index, length_percent, &ex, &ey);
    oledC_DrawLine(CX, CY, ex, ey, 1, color);
}

static void draw_hand_medium(uint8_t index, uint8_t length_percent, uint16_t color)
{
    uint8_t ex, ey;
    get_scaled_point(index, length_percent, &ex, &ey);

    oledC_DrawLine(CX, CY, ex, ey, 1, color);
    oledC_DrawLine(CX + 1, CY, ex + 1, ey, 1, color);
}

static void draw_hand_thick(uint8_t index, uint8_t length_percent, uint16_t color)
{
    uint8_t ex, ey;
    get_scaled_point(index, length_percent, &ex, &ey);

    oledC_DrawLine(CX, CY, ex, ey, 1, color);
    oledC_DrawLine(CX + 1, CY, ex + 1, ey, 1, color);
    oledC_DrawLine(CX, CY + 1, ex, ey + 1, 1, color);
}

static void draw_clock_marks(void)
{
    int i;

    for (i = 0; i < 60; i++)
    {
        uint8_t sx, sy, ex, ey;

        if (i % 5 == 0)
        {
            get_scaled_point(i, 78, &sx, &sy);
            get_scaled_point(i, 100, &ex, &ey);
            oledC_DrawLine(sx, sy, ex, ey, 1, 0xFFFF);
        }
        else
        {
            get_scaled_point(i, 88, &sx, &sy);
            get_scaled_point(i, 100, &ex, &ey);
            oledC_DrawLine(sx, sy, ex, ey, 1, 0xFFFF);
        }
    }
}

static void draw_clock_outline(void)
{
    oledC_DrawCircle(CX, CY, CLOCK_R, 0xFFFF);
}

static void draw_center_dot(void)
{
    oledC_DrawPoint(CX, CY, 0xFFFF);
    oledC_DrawPoint(CX + 1, CY, 0xFFFF);
    oledC_DrawPoint(CX, CY + 1, 0xFFFF);
    oledC_DrawPoint(CX + 1, CY + 1, 0xFFFF);
}

void update_analog_face(void)
{
    uint8_t h_index;
    uint16_t c_h = 0xFFFF;
    uint16_t c_m = 0xFFFF;
    uint16_t c_s = 0xFFFF;

    h_index = (now.hour % 12) * 5 + (now.min / 12);

    if (analogTheme == 1)
    {
        c_h = OLEDC_COLOR_RED;
        c_m = OLEDC_COLOR_BLUE;
        c_s = OLEDC_COLOR_GREEN;
    }

    /* clear full screen / analog area */
    oledC_DrawRectangle(0, 0, 95, 95, 0x0000);

    draw_clock_outline();
    draw_clock_marks();

    draw_hand_thick(h_index, 45, c_h);
    draw_hand_medium(now.min, 65, c_m);
    draw_hand_thin(now.sec, 85, c_s);

    draw_center_dot();
}