#include "myExternalLED.h"
#include "myCommonFunctions.h"
#include "myOSFunctions.h"

port_c_t greenLEDPins[NUM_GREEN_LEDS] = {PTC3, PTC4, PTC5, PTC6, PTC10, PTC11, PTC12, PTC13, PTC16, PTC17};
int is_in_motion = 0;

void initExternalLED(){
    int i = 0;
    // Enable Clock to PORTA and PORTC
    SIM->SCGC5 |= ((SIM_SCGC5_PORTA_MASK) | (SIM_SCGC5_PORTC_MASK));

    // Initalize GPIO Pins for the Green LED Strip
    for(i = 0; i < NUM_GREEN_LEDS; i++){
        initGPIOPin(&(PORTC->PCR[greenLEDPins[i]]), &(PTC->PDDR), greenLEDPins[i], 1);
    }

    // Initalize GPIO Pins for the Red LED Strip
    initGPIOPin(&(PORTA->PCR[PTA17]), &(PTA->PDDR), PTA17, 1);
}

void toggleRedLED() {
    PTA->PTOR = MASK(PTA17);
}

void onGreenLED(int led){
    PTC->PSOR = MASK(greenLEDPins[led]);
}

void offGreenLED(int led){
    PTC->PCOR = MASK(greenLEDPins[led]);
}

void clearLitGreenLED(){
    int i;
    for(i = 0; i < NUM_GREEN_LEDS; i++) {
        setInactiveGreenLED(i);
    }
}

void nextTrailGreenLED() {
    static int cur_led = 0;
    offGreenLED(cur_led);
    cur_led++;
    cur_led %= NUM_GREEN_LEDS;
    onGreenLED(cur_led);
}

void setAllGreenLED() {
    int i;    
    for(i = 0; i < NUM_GREEN_LEDS; i++) {
        PTC->PSOR = MASK(greenLEDPins[i]);
    }
}

void signalSuccessConnection() {
    int reps_left = 2;
    while(reps_left) {
        setAllGreenLED();
        osDelay(CONNECTION_FLASH_DELAY);
        clearLitGreenLED();

        reps_left--;
        osDelay(CONNECTION_REPS_DELAY);
    }	
}

void runStationeryLED(){
    is_in_motion = 0;
}

void runMotionLED(){
    is_in_motion = 1;
}

void runExternalLED(){
    // Moving
    if (is_in_motion){
        clearLitGreenLED();
 
        nextTrailGreenLED();
        toggleRedLED();
        osDelay(RED_RUN_DELAY);
        
    // Stationary
    } else {  
        setAllGreenLED();
        
        toggleRedLED();
        osDelay(RED_STOP_DELAY);
    }
}
