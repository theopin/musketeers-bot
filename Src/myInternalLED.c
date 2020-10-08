#include "myInternalLED.h"
#include "myCommonFunctions.h"

led_colors_t color_sets[3] = {RED, GREEN, BLUE};

void initInternalLED(){
    // Enable Clock to PORTB and PORTD
    SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTD_MASK));
    
    // Configure MUX settings to make all 3 pins GPIO
    initGPIOPin(&(PORTB->PCR[RED_LED]), &(PTB->PDDR), RED_LED, 1);
    initGPIOPin(&(PORTB->PCR[GREEN_LED]), &(PTB->PDDR), GREEN_LED, 1);
    initGPIOPin(&(PORTD->PCR[BLUE_LED]), &(PTD->PDDR), BLUE_LED, 1);
    offLED();
}

void toggleLED(led_colors_t color) {
    offLED();

    if(color == BLUE_LED)
        PTD->PCOR = MASK(color);

    // Color is Red/Green
    else
        PTB->PCOR = MASK(color);
}



