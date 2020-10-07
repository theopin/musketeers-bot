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

void initInternalLED();

void toggleLED(led_colors_t color) {
    offLED();

    if(color == BLUE_LED)
        PTD->PCOR = MASK(color);

    // Color is Red/Green
    else
        PTB->PCOR = MASK(color);
}



