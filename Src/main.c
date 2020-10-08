/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
#include "myCommonFunctions.h"
#include "myOSFunctions.h"
#include "myExternalLED.h"
#include "myInternalLED.h"
#include "myMotor.h"
#include "myUART.h"
#include "myMessageList.h"
//#include "myAudio.h"

uint8_t message = 0; // TODO DEFINE BELOW AGAIN, HERE FOR DEBUG	
int a = 0;
int b = 0;
int e = 0;
int f = 0;

osEventFlagsId_t event_flags_id;
/*----------------------------------------------------------------------------
 * Thread - LED Control
 *---------------------------------------------------------------------------*/
void tLED(void *argument) {
    for (;;) {
        // Any set Motor bits in event flag signal indicate a moving robot
        static int isRobotMoving = 0;
        if (osEventFlagsGet(event_flags_id) & EXT_LED_BOT_STATIONERY_EF_MASK) {
            osEventFlagsClear(event_flags_id, EXT_LED_BOT_STATIONERY_EF_MASK);
            isRobotMoving = 0;
        } else if (osEventFlagsGet(event_flags_id) & EXT_LED_BOT_MOVING_EF_MASK) {
            osEventFlagsClear(event_flags_id, EXT_LED_BOT_MOVING_EF_MASK);
            isRobotMoving = 1;
        }
        
        e = osEventFlagsGet(event_flags_id);
        
        // Handles the event
        switch(isRobotMoving) {
        // Stationery
        case 0:  
            a++;
            setStationeryLED();
            break;
        
        // Moving
        case 1:  
            b++;
            setMovingLED();
            break;
        
        default:
            f++;
            setStationeryLED();
            break;
        }
        runExternalLED();
    }
}

/*----------------------------------------------------------------------------
 * Thread - Motor Control
 *---------------------------------------------------------------------------*/
void tMotorControl(void *argument) {
    uint32_t flags;
    for (;;) {
        // Wait till motor specific flags are set but does not clear them
        flags = MOTOR_EF_MASK & osEventFlagsWait(event_flags_id, MOTOR_EF_MASK, osFlagsWaitAny, osWaitForever);
        // Handles the event
        switch(flags) {
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
    
    for (;;) {
      //  uint8_t message;
        Q_T* rxQ = getReceiveBuffer();
        Q_Enqueue( rxQ, MESSAGE_STOP); //debug
        if (!Q_Empty(rxQ)) {
            message = Q_Dequeue(rxQ);
            
            osEventFlagsClear(event_flags_id, 0xFFFF); // clears all flags
            
            /* We set the appropriate flag to indicate a task needing handling
              ,the relevant tasks will use this flag to know
              when and what to run. */
            switch (message) {
            // Movement
            case MESSAGE_STOP:
                osEventFlagsSet(event_flags_id, EXT_LED_BOT_STATIONERY_EF_MASK);
                osEventFlagsSet(event_flags_id, message);
                break;
            case MESSAGE_N:  // Fallthrough
            case MESSAGE_NE: // Fallthrough
            case MESSAGE_E:  // Fallthrough
            case MESSAGE_SE: // Fallthrough
            case MESSAGE_S:  // Fallthrough
            case MESSAGE_SW: // Fallthrough
            case MESSAGE_W:  // Fallthrough
            case MESSAGE_NW: 
                osEventFlagsSet(event_flags_id, EXT_LED_BOT_MOVING_EF_MASK);
                osEventFlagsSet(event_flags_id, message);
                break;	
            
            // Bluetooth
            case MESSAGE_BT_CONNECT:
                osEventFlagsSet(event_flags_id, BT_EF_MASK); 			
                break;	

                // toggleLED(GREEN);
                //   signalSuccessConnection();
                //				osThreadNew(greenLEDThread, NULL, NULL);
                //				osThreadNew(redLEDThread, NULL, NULL);
                //				break;
            
            // Bad command
            default:
                osEventFlagsSet(event_flags_id, EXT_LED_BOT_STATIONERY_EF_MASK);
                osEventFlagsSet(event_flags_id, MESSAGE_STOP);				
                break;
            }

            // This task is BLOCKED until all the flags have been handled or TIME OUT
             osEventFlagsWait(event_flags_id, 0x0, osFlagsWaitAll, EVENT_TIME_OUT); 
        } 
    }
}

// Initialize event flags to synchornize threads
void initEvents() {
    event_flags_id = osEventFlagsNew(NULL);
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

    osKernelInitialize();				 // Initialize CMSIS-RTOS
    initEvents();                        // Note must be done AFTER initializing kernel
    osThreadNew(tBrain, NULL, NULL);
    osThreadNew(tMotorControl, NULL, NULL);
    osThreadNew(tAudio,  NULL, NULL);
    osThreadNew(tLED, NULL, NULL);
    osKernelStart();					  // Start thread execution
    for (;;) {}
}
