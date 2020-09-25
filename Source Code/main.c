#include "myLEDBlinky.h"
#include "myUART.h"

int main(void) {
  
    SystemCoreClockUpdate();    
    initExternalLEDPins();
   
    while(1) {
		signalSuccessConnection();
        delay(10000000);
        //operateGreenLED();
		//operateRedLED();
    }
}
