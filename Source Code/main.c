/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_os2.h"

#include "myExternalLED.h"
#include "myUART.h"
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
        osDelay(1000);
    }
}

/*----------------------------------------------------------------------------
 * Thread - Red LED
 *---------------------------------------------------------------------------*/
void redLEDThread (void *argument) {
    for (;;) {
        operateRedLED();
        osDelay(1000);
    }
}

int main (void) {
 
    // System Initialization
    SystemCoreClockUpdate();
    initRobotComponents();
 
    osKernelInitialize();                 // Initialize CMSIS-RTOS
  
    osThreadNew(greenLEDThread, NULL, NULL);
    osThreadNew(redLEDThread, NULL, NULL);
  
    osKernelStart();                      // Start thread execution
    for (;;) {}
}
