#include <MKL25Z4.h>

#define MASK(x) (1 << (x))
#define EVENT_TIME_OUT 100000

void initGPIOPin(__IO uint32_t *PCR, __IO uint32_t *PDDR, int pin, int isOutput);
double max(double val1, double val2);
