#pragma once

#include <MKL25Z4.h>

#define MASK(x) (1 << (x))
#define EVENT_TIME_OUT 10000

static void delay(volatile uint32_t nof) {
    while(nof!=0) {
        __asm("NOP");
        nof--;
    }
}

void initGPIOPin(__IO uint32_t *PCR, __IO uint32_t *PDDR, int pin, int isOutput)
{
    *PCR &= ~PORT_PCR_MUX_MASK;
    *PCR |= PORT_PCR_MUX(1);

    if(isOutput)
        *PDDR |= (MASK(pin));
    // Input
    else
        *PDDR &= ~(MASK(pin));
}
