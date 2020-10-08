#include "myInternalLED.h"
#include "myCommonFunctions.h"

led_colors_t color_sets[3] = {RED, GREEN, BLUE};

void offInternalLED() {  
    PTD->PSOR = MASK(BLUE_LED);
    PTB->PSOR = (MASK(RED_LED) | MASK(GREEN_LED));
}

void initInternalLED(void) {
    // Enable Clock to PORTB and PORTD
    SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTD_MASK));

    // Configure MUX settings to make all 3 pins GPIO
    initGPIOPin(&(PORTB->PCR[RED_LED]), &(PTB->PDDR), RED_LED, 1);
    initGPIOPin(&(PORTB->PCR[GREEN_LED]), &(PTB->PDDR), GREEN_LED, 1);
    initGPIOPin(&(PORTD->PCR[BLUE_LED]), &(PTD->PDDR), BLUE_LED, 1);
    offInternalLED();
}

void toggleLED(led_colors_t color) {
    offInternalLED();

    if(color == BLUE_LED) {
        PTD->PCOR = MASK(color);
    } else { // Color is Red/Green
        PTB->PCOR = MASK(color);
    }
}



