#include <MKL25Z4.h>

#define MASK(x) (1 << (x))
#define EVENT_TIME_OUT 10000

static void delay(volatile uint32_t nof);
void initGPIOPin(__IO uint32_t *PCR, __IO uint32_t *PDDR, int pin, int isOutput);