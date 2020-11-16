#include "myExternalLED.h"
#include "myCommonFunctions.h"
#include "myOSFunctions.h"

static port_c_t greenLEDPins[NUM_GREEN_LEDS] = {PTC3, PTC4, PTC5, PTC6, PTC10, PTC11, PTC12, PTC13, PTC16, PTC17};
static int is_moving = 0;
static int is_connected = 0;

void initExternalLED() {
    int i = 0;
    // Enable Clock to PORTA and PORTC
    SIM->SCGC5 |= ((SIM_SCGC5_PORTA_MASK) | (SIM_SCGC5_PORTC_MASK));

    // Initalize GPIO Pins for the Green LED Strip
    for(i = 0; i < NUM_GREEN_LEDS; i++) {
        initGPIOPin(&(PORTC->PCR[greenLEDPins[i]]), &(PTC->PDDR), greenLEDPins[i], 1);
    }

    // Initalize GPIO Pins for the Red LED Strip
    initGPIOPin(&(PORTA->PCR[PTA17]), &(PTA->PDDR), PTA17, 1);
}

void toggleRedLED(void) {
    PTA->PTOR = MASK(PTA17);
}

void onGreenLED(int led) {
    PTC->PSOR = MASK(greenLEDPins[led]);
}

void offGreenLED(int led) {
    PTC->PCOR = MASK(greenLEDPins[led]);
}

void clearAllGreenLED() {
    int i;
    for(i = 0; i < NUM_GREEN_LEDS; i++) {
        offGreenLED(i);
    }
}

void nextTrailGreenLED(void) {
    static int cur_led = 0;
    offGreenLED(cur_led);
    cur_led++;
    cur_led %= NUM_GREEN_LEDS;
    onGreenLED(cur_led);
}

void setAllGreenLED(void) {
    int i;    
    for(i = 0; i < NUM_GREEN_LEDS; i++) {
        PTC->PSOR = MASK(greenLEDPins[i]);
    }
}

void signalSuccessConnection(void) {
    int reps_left = 2;
    while(reps_left) {
        setAllGreenLED();
        osDelay(CONNECTION_FLASH_DELAY);
        clearAllGreenLED();

        reps_left--;
        osDelay(CONNECTION_REPS_DELAY);
    }
    
    setIsConnected(1);
}

void setStationeryLED() {
    is_moving = 0;
}

void setMovingLED() {
    is_moving = 1;
}

int getIsMoving() {
    return is_moving;
}

void runExternalMovingLED() {
    clearAllGreenLED();

    nextTrailGreenLED();
    toggleRedLED();
    osDelay(RED_RUN_DELAY);
}

void runExternalStationeryLED() {
    setAllGreenLED();

    toggleRedLED();
    osDelay(RED_STOP_DELAY);
}

int getIsConnected() {
    return is_connected;
}

void setIsConnected(int val) {
    is_connected = val;
}
