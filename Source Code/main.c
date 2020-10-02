/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "myOSFunctions.h"
#include "myInternalLED.h"
#include "myMessageList.h" 

void initRobotComponents(void) {
    initUART2(BAUD_RATE);
    initExternalLED();
    initInternalLED();
}    
 
/*----------------------------------------------------------------------------
 * Thread - Green LED
 *---------------------------------------------------------------------------*/
void greenLEDThread (void *argument) {
  
    for (;;) {
        operateGreenLED();
    }
}

/*----------------------------------------------------------------------------
 * Thread - Red LED
 *---------------------------------------------------------------------------*/
void redLEDThread (void *argument) {
    for (;;) {
        operateRedLED();
    }
}

/*----------------------------------------------------------------------------
 * Thread - Communications
 *---------------------------------------------------------------------------*/
void commsThread (void *argument) {
    uint8_t message;
    for (;;) {
        message = operateUART2();
        switch(message) {
            case MESSAGE_E:
                osThreadNew(greenLEDThread, NULL, NULL);
                osThreadNew(redLEDThread, NULL, NULL);
                break;       
        }
    }
}

int main (void) {
 
    // System Initialization
    SystemCoreClockUpdate();
    initRobotComponents();
 
    osKernelInitialize();                 // Initialize CMSIS-RTOS
  
    //osThreadNew(commsThread, NULL, NULL);
    osThreadNew(greenLEDThread, NULL, NULL);
    osThreadNew(redLEDThread, NULL, NULL);
    
    osKernelStart();                      // Start thread execution
    for (;;) {}
}
