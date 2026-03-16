#include "h/watch.h"

#define CX 47
#define CY 47
#define RADIUS 36

extern uint8_t analogTheme;

static uint16_t get_part_color(uint8_t part) {
    // part: 0=marks, 1=hour, 2=min, 3=sec
    if (analogTheme == 0) { 
        if (part == 3) return 0xF800; // Red sec
        return 0xFFFF;                // White rest
    }
    if (analogTheme == 1) { 
        if (part == 0) return 0x07FF; // Cyan marks
        if (part == 3) return 0xF81F; // Magenta sec
        return 0xFFFF;                // White hands
    }
    if (analogTheme == 2) { 
        if (part == 0) return 0xFFE0; // Yellow marks
        if (part == 1) return 0xF800; // Red hour
        if (part == 2) return 0x07E0; // Green min
        return 0x001F;                // Blue sec
    }
    return 0xFFFF;
}

const uint8_t edge_x[60] = {
    47, 52, 56, 61, 65, 70, 74, 78, 81, 85,
    87, 90, 92, 93, 94, 94, 94, 93, 92, 90,
    87, 85, 81, 78, 74, 70, 65, 61, 56, 52,
    47, 42, 38, 33, 29, 24, 20, 16, 13, 9,
    7, 4, 2, 1, 0, 0, 0, 1, 2, 4,
    7, 9, 13, 16, 20, 24, 29, 33, 38, 42
};

const uint8_t edge_y[60] = {
    0, 0, 1, 2, 4, 7, 9, 13, 16, 20,
    24, 29, 33, 38, 42, 47, 52, 56, 61, 65,
    70, 74, 78, 81, 85, 87, 90, 92, 93, 94,
    94, 94, 93, 92, 90, 87, 85, 81, 78, 74,
    70, 65, 61, 56, 52, 47, 42, 38, 33, 29,
    24, 20, 16, 13, 9, 7, 4, 2, 1, 0
};

static void point_on_radius(uint8_t index, uint8_t percent, uint8_t *x, uint8_t *y)
{
    int dx = (int)edge_x[index] - CX;
    int dy = (int)edge_y[index] - CY;

    *x = (uint8_t)(CX + ((long)dx * RADIUS * percent) / (47L * 100L));
    *y = (uint8_t)(CY + ((long)dy * RADIUS * percent) / (47L * 100L));
}

static void draw_line_pixels(int x0, int y0, int x1, int y1, uint16_t color)
{
    int dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int sx = (x0 < x1) ? 1 : -1;
    int dy = (y1 > y0) ? -(y1 - y0) : -(y0 - y1);
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    while (1)
    {
        if (x0 >= 0 && x0 < 96 && y0 >= 0 && y0 < 96)
        {
            oledC_DrawPoint((uint8_t)x0, (uint8_t)y0, color);
        }

        if (x0 == x1 && y0 == y1) break;

        {
            int e2 = 2 * err;
            if (e2 >= dy) { err += dy; x0 += sx; }
            if (e2 <= dx) { err += dx; y0 += sy; }
        }
    }
}

static void draw_line_thick(int x0, int y0, int x1, int y1, uint8_t width, uint16_t color)
{
    draw_line_pixels(x0, y0, x1, y1, color);

    if (width >= 2)
    {
        draw_line_pixels(x0 + 1, y0, x1 + 1, y1, color);
        draw_line_pixels(x0, y0 + 1, x1, y1 + 1, color);
    }

    if (width >= 3)
    {
        draw_line_pixels(x0 - 1, y0, x1 - 1, y1, color);
        draw_line_pixels(x0, y0 - 1, x1, y1 - 1, color);
    }
}

static void draw_center_dot(void)
{
    uint16_t c = get_part_color(0);
    oledC_DrawPoint(CX,     CY,     c);
    oledC_DrawPoint(CX + 1, CY,     c);
    oledC_DrawPoint(CX,     CY + 1, c);
    oledC_DrawPoint(CX + 1, CY + 1, c);
}

static void draw_clock_marks(void)
{
    uint16_t c = get_part_color(0);
    for (uint8_t i = 0; i < 60; i++)
    {
        uint8_t sx, sy, ex, ey;

        if (i % 5 == 0) point_on_radius(i, 90, &sx, &sy);
        else point_on_radius(i, 95, &sx, &sy);

        point_on_radius(i, 100, &ex, &ey);
        draw_line_thick((int)sx, (int)sy, (int)ex, (int)ey, 1, c);
    }
}

static void draw_hand(uint8_t index, uint8_t percent, uint8_t width, uint16_t color)
{
    uint8_t ex, ey;
    point_on_radius(index, percent, &ex, &ey);
    draw_line_thick(CX, CY, (int)ex, (int)ey, width, color);
}

static void draw_face_static(void)
{
    draw_clock_marks();
    draw_center_dot();
}

void update_analog_face(void)
{
    static uint8_t first_draw = 1;
    static uint8_t old_s = 0;
    static uint8_t old_m = 0;
    static uint8_t old_h = 0;

    uint8_t new_s = now.sec;
    uint8_t new_m = now.min;
    uint8_t new_h = (uint8_t)((now.hour % 12) * 5 + (now.min / 12));

    if (first_draw || g_force_redraw)
    {
        draw_face_static();

        draw_hand(new_h, 50, 2, get_part_color(1));
        draw_hand(new_m, 80, 1, get_part_color(2));
        draw_hand(new_s, 90, 1, get_part_color(3));

        old_h = new_h;
        old_m = new_m;
        old_s = new_s;

        first_draw = 0;
        g_force_redraw = false;
        return;
    }

    if (new_s == old_s) return;

    // Erase old hands with black
    draw_hand(old_h, 50, 2, 0x0000);
    draw_hand(old_m, 80, 1, 0x0000);
    draw_hand(old_s, 90, 1, 0x0000);

    // Restore marks and center
    draw_clock_marks();
    draw_center_dot();

    // Draw new hands
    draw_hand(new_h, 50, 2, get_part_color(1));
    draw_hand(new_m, 80, 1, get_part_color(2));
    draw_hand(new_s, 90, 1, get_part_color(3));

    old_h = new_h;
    old_m = new_m;
    old_s = new_s;
}