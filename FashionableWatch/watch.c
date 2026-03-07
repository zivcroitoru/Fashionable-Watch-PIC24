#include "h/watch.h"
#include "../System/system.h"
#include "../oledDriver/oledC.h"
#include "../oledDriver/oledC_shapes.h" 
#include "../oledDriver/oledC_colors.h" 

#include <xc.h>

uint8_t analogTheme = 0;

void init_all(void) {
    SYSTEM_Initialize();
    oledC_setup();
    
    // 1. PIN CONFIGURATION (Buttons & LEDs)
    // S1 = RA11, S2 = RA12
    TRISAbits.TRISA11 = 1;  // S1 as Input
    
    TRISAbits.TRISA12 = 1;  // S2 as Input
    
    // LED1 = RA0, LED2 = RA1
    TRISAbits.TRISA0 = 0;   // LED1 as Output
    ANSELAbits.ANSA0 = 0;   // LED1 as Digital
    
    TRISAbits.TRISA1 = 0;   // LED2 as Output
    ANSELAbits.ANSA1 = 0;   // LED2 as Digital
    
    // 2. TIMER 1 SETUP (1 second interval)
    T1CON = 0x8030;         
    PR1 = 15625;            
    _T1IF = 0; 
    _T1IE = 1; 
    
    // 3. INITIAL SCREEN CLEAR
    oledC_DrawRectangle(0, 0, 95, 95, 0x0000); 
}