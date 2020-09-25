#include "myLEDBlinky.h"
#include "myUART.h"
#include "myInternalLED.h"

int main(void) {
  
    SystemCoreClockUpdate();    
    initUART2(BAUD_RATE);
    initExternalLEDPins();
    initInternalLED();
    offLED();
   
    while(1) {
		uint8_t message;
        if (!Q_Empty(&rxQ)) {
            message = Q_Dequeue(&rxQ);
            if (message == 0x03)
                toggleLED(RED);
            else if (message == 0x02)
                toggleLED(GREEN);
            else 
                toggleLED(BLUE);
 
            
        }
        
    }
}
