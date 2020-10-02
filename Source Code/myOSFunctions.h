#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_os2.h"

#include "myExternalLED.h"
#include "myUART.h"

void operateGreenLED() {
	// Green LED Version
    if(isRunning){
        clearLitGreenLED();
        setTrailGreenLED();
    }
    else
        setAllGreenLED();
}

void operateRedLED() {
    uint32_t redDelayLength;
	toggleRedLED();
    redDelayLength = isRunning ? RED_RUN_DELAY : RED_STOP_DELAY;
    delay(redDelayLength);
}

uint8_t operateUART2() {
    if (!Q_Empty(&rxQ)) 
        return Q_Dequeue(&rxQ);

    return NULL;
}
