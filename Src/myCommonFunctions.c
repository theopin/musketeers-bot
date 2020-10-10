#include "myCommonFunctions.h"
#include <MKL25Z4.h>

void initGPIOPin(__IO uint32_t *PCR, __IO uint32_t *PDDR, int pin, int isOutput){
    *PCR &= ~PORT_PCR_MUX_MASK;
    *PCR |= PORT_PCR_MUX(1); 

    if(isOutput){
        *PDDR |= (MASK(pin));
    // Input
    } else {
        *PDDR &= ~(MASK(pin));
    }
}

double max(double val1, double val2) {
    if (val1 > val2) {
        return val1;
    } else {
        return val2;
    }
}
