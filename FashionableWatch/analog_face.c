#include "h/watch.h"
#define CX 47
#define CY 47

const uint8_t edge_x[60] = {47, 52, 56, 61, 65, 70, 74, 78, 81, 85, 87, 90, 92, 93, 94, 94, 94, 93, 92, 90, 87, 85, 81, 78, 74, 70, 65, 61, 56, 52, 47, 42, 38, 33, 29, 24, 20, 16, 13, 9, 7, 4, 2, 1, 0, 0, 0, 1, 2, 4, 7, 9, 13, 16, 20, 24, 29, 33, 38, 42};
const uint8_t edge_y[60] = {0, 0, 1, 2, 4, 7, 9, 13, 16, 20, 24, 29, 33, 38, 42, 47, 52, 56, 61, 65, 70, 74, 78, 81, 85, 87, 90, 92, 93, 94, 94, 94, 93, 92, 90, 87, 85, 81, 78, 74, 70, 65, 61, 56, 52, 47, 42, 38, 33, 29, 24, 20, 16, 13, 9, 7, 4, 2, 1, 0};

// Updated with the 'width' parameter
void draw_clock_hand(uint8_t index, uint8_t length_percent, uint8_t width, uint16_t color) {
    uint8_t end_x = CX + ((edge_x[index] - CX) * length_percent) / 100;
    uint8_t end_y = CY + ((edge_y[index] - CY) * length_percent) / 100;
    oledC_DrawLine(CX, CY, end_x, end_y, width, color); 
}

void update_analog_face(void) {
    static uint8_t old_s = 60, old_m = 60, old_h = 60;
    uint8_t h_index = (now.hour % 12) * 5 + (now.min / 12);

    if (old_s == 60) { old_s = now.sec; old_m = now.min; old_h = h_index; }

    // Erase old hands (Draw black with specific widths)
    draw_clock_hand(old_s, 90, 1, 0x0000); 
    draw_clock_hand(old_m, 80, 2, 0x0000); 
    draw_clock_hand(old_h, 50, 3, 0x0000); 

    // Redraw marks based on theme
    uint8_t step = (analogTheme == 2) ? 15 : 5; 
    for(int i = 0; i < 60; i += step) {
        uint8_t sx = CX + ((edge_x[i] - CX) * 90) / 100;
        uint8_t sy = CY + ((edge_y[i] - CY) * 90) / 100;
        oledC_DrawLine(sx, sy, edge_x[i], edge_y[i], 1, 0xFFFF); // Width 1
    }

    // Set colors based on theme
    uint16_t c_h = 0xFFFF, c_m = 0xFFFF, c_s = 0xFFFF;
    if (analogTheme == 1) {
        c_h = OLEDC_COLOR_RED;   
        c_m = OLEDC_COLOR_BLUE;  
        c_s = OLEDC_COLOR_GREEN; 
    }

    // Draw new hands (Hour = 3 width, Min = 2 width, Sec = 1 width)
    draw_clock_hand(h_index, 50, 3, c_h); 
    draw_clock_hand(now.min, 80, 2, c_m); 
    draw_clock_hand(now.sec, 90, 1, c_s); 

    old_s = now.sec;
    old_m = now.min;
    old_h = h_index;
}