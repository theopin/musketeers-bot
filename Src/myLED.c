#include "myInternalLED.h"
#include "myExternalLED.h"

volatile int isLedTrailing = 0;

void updateExternalLED(){
    if(isLedTrailing){
        clearLitGreenLED();
        setTrailGreenLED();
    }
    else{
        setAllGreenLED();
    }
}

void operateRedLED() {
    uint32_t redDelayLength;
    toggleRedLED();
    redDelayLength = isLedTrailing ? RED_RUN_DELAY : RED_STOP_DELAY;
    delay(redDelayLength);
}