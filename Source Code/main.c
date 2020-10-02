/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "myOSFunctions.h"
#include "myInternalLED.h"


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
        if(message == MESSAGE_BT_CONNECT) {
            toggleLED(GREEN);
            signalSuccessConnection();
            osThreadNew(greenLEDThread, NULL, NULL);
            osThreadNew(redLEDThread, NULL, NULL);
        }
        else if (message == MESSAGE_NE)
            toggleLED(RED); 
      
    }
}

int main (void) {
 
    // System Initialization
    SystemCoreClockUpdate();
    initRobotComponents();
 
    osKernelInitialize();                 // Initialize CMSIS-RTOS
  
    osThreadNew(commsThread, NULL, NULL);

    osKernelStart();                      // Start thread execution
    for (;;) {}

/*    
    while(1) {
		uint8_t message;
        if (!Q_Empty(&rxQ)) {
            message = Q_Dequeue(&rxQ);
            if (message == MESSAGE_E)
                toggleLED(RED);
            else if (message == 0x02)
                toggleLED(GREEN);
            else 
                toggleLED(BLUE);        
        }
    }    
*/
}

