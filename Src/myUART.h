#include "MKL25Z4.h"
#include "myCircularBuffer.h"

#define BAUD_RATE 9600
#define UART_TX_PTE22 22
#define UART_RX_PTE23 23
#define UART2_INT_PRIO 128

Q_T rxQ;

void initUART2(uint32_t baud_rate);