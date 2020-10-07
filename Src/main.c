/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#include "myOSFunctions.h"
#include "myLED.h"
#include "myMotor.h"
//#include "myAudio.h"
#include "myUART.h"
#include "myMessageList.h"

osEventFlagsId_t events_flags_id;
osEventFlagsId_t _id;  
osEventFlagsId_t event_id;  
/*----------------------------------------------------------------------------
 * Thread - LED Control
 *---------------------------------------------------------------------------*/
void tLED(void *argument) {
    uint32_t flags;
    
    for (;;) {
        // Wait till motor specific flags are set
        flags = osEventFlagsWait(events_flags_id, MESSAGE_BACK_LED_EVENT_FLAG_MASK, osFlagsWaitAny, osWaitForever);

        // Handles the event
        switch(flags){
            case MESSAGE_STOP:
                stop();
                break;  
            case MESSAGE_N:
                moveN();
                break;
            case MESSAGE_NE:
                moveNE();
                break;
            case MESSAGE_E:
                moveE();
                break;
            case MESSAGE_SE:
                moveSE();
                break;
            case MESSAGE_S:
                moveS();
                break;
            case MESSAGE_SW:
                moveSW();
                break;
            case MESSAGE_W:
                moveW();
                break;
            case MESSAGE_NW:
                moveNW();			
                break;
            default:
                stop();
                break;
        }
        operateExternalLED();
    }
}

/*----------------------------------------------------------------------------
 * Thread - Motor Control
 *---------------------------------------------------------------------------*/
void tMotorControl(void *argument) {
    uint32_t flags;
    for (;;) {
        // Wait till motor specific flags are set
        flags = osEventFlagsWait(events_flags_id, MESSAGE_MOTOR_EVENT_FLAG_MASK, osFlagsWaitAny, osWaitForever);

        // Handles the event
        switch(flags){
            case MESSAGE_STOP:
                stop();
                break;  
            case MESSAGE_N:
                moveN();
                break;
            case MESSAGE_NE:
                moveNE();
                break;
            case MESSAGE_E:
                moveE();
                break;
            case MESSAGE_SE:
                moveSE();
                break;
            case MESSAGE_S:
                moveS();
                break;
            case MESSAGE_SW:
                moveSW();
                break;
            case MESSAGE_W:
                moveW();
                break;
            case MESSAGE_NW:
                moveNW();			
                break;
            default:
                stop();
                break;
        }
    }
}

/*----------------------------------------------------------------------------
 * Thread - Audio Control
 *---------------------------------------------------------------------------*/
void tAudio(void *argument) {
    for (;;) {
    }
}


/*----------------------------------------------------------------------------
 * Thread - Serial Data Decoder
 *---------------------------------------------------------------------------*/
void tBrain(void *arguement) {
    uint8_t message;	
    for (;;) {
        if (!Q_Empty(&rxQ)) {
            message = Q_Dequeue(&rxQ);

            /* We set the appropriate flag, the relevant tasks will use this flag to know
               when an what to run. */
            switch (message){
                // Movement
                case MESSAGE_STOP:
                case MESSAGE_N:
                case MESSAGE_NE:
                case MESSAGE_E:
                case MESSAGE_SE:
                case MESSAGE_S:
                case MESSAGE_SW:
                case MESSAGE_W:
                case MESSAGE_NW:
                    osEventFlagsSet(event_id, message); 			
                    break;	
                    // Bluetooth
                case MESSAGE_BT_CONNECT:
                    osEventFlagsSet(event_id, MESSAGE_BT_EVENT_FLAG_MASK); 			
                    break;	

                    // toggleLED(GREEN);
                    //   signalSuccessConnection();
                    //				osThreadNew(greenLEDThread, NULL, NULL);
                    //				osThreadNew(redLEDThread, NULL, NULL);
                    //				break;
                    // Bad command
                default:
                    osEventFlagsSet(event_id, MESSAGE_STOP); 
                    //   stop(); 					
                    break;
            }

            // This task is BLOCKED until all the flags have been handled or TIME OUT
            osEventFlagsWait(events_flags_id, 0x0, osFlagsWaitAll, EVENT_TIME_OUT); 
        } 
    }
}

void initRobotComponents(void) {
    initUART2(BAUD_RATE);
    initExternalLED();
    initInternalLED();
}	

int main(void) {
    // System Initialization
    SystemCoreClockUpdate();
    initRobotComponents();
    initPWM();

    events_flags_id = osEventFlagsNew(NULL); // Initialize event flags to synchornize threads

    osKernelInitialize();				 // Initialize CMSIS-RTOS
    osThreadNew(tBrain, NULL, NULL);
    osThreadNew(tMotorControl, NULL, NULL);
    osThreadNew(tAudio,  NULL, NULL);
    osThreadNew(tLED, NULL, NULL);
    osKernelStart();					  // Start thread execution
    for (;;) {}
}
