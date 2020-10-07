#include "myCommonFunctions.h"

#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1
#define NUM_LEDS 3

typedef enum {
    RED = RED_LED, 
    GREEN = GREEN_LED, 
    BLUE = BLUE_LED
}led_colors_t;

led_colors_t color_sets[3] = {RED, GREEN, BLUE};

void offLED() {  
    PTD->PSOR = MASK(BLUE_LED);
    PTB->PSOR = (MASK(RED_LED) | MASK(GREEN_LED));
}

void initInternalLED() {
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



