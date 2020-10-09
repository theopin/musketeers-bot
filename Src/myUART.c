#include "myUART.h"
#include "MKL25Z4.h"
#include "myMessageList.h"

Q_T rxQ;

Q_T* getReceiveBuffer(void) {
    return &rxQ;
}

void initUART2(uint32_t baud_rate) {
    uint32_t divisor, bus_clock;

    // Enable clock to Port E and UART 2
    SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

    // 4 - Alt 4 Location of UART2 -TX/RX
    PORTE->PCR[UART_RX_PTE23] &= ~PORT_PCR_MUX_MASK;
    PORTE->PCR[UART_RX_PTE23] |= PORT_PCR_MUX(4);

    // Disable transmitter and Receiver
    UART2->C2 &= ~((UART_C2_TE_MASK) | (UART_C2_RE_MASK));

    /*
     * busClock: runs at half the rate of systemClock
     * UART: runs based on bus clock
     * divisor: Scale to divide by - include oversampling
     * UART_BDH_SBR; Upper bits of the divisor
     * UART_BDL_SBR; Lower bits of the divisor
     */
    bus_clock = (DEFAULT_SYSTEM_CLOCK)/2;
    divisor = bus_clock / (baud_rate * 16);
    UART2->BDH = UART_BDH_SBR(divisor >> 8);
    UART2->BDL = UART_BDL_SBR(divisor);

    // No parity, 8 bits, one stop bits, other settings;
    UART2->C1 = 0;
    UART2->S2 = 0;
    UART2->C3 = 0;

    Q_Init(&rxQ); // Initialize circular buffer

    // Interrupt configuration
    NVIC_SetPriority(UART2_IRQn, 2);

    NVIC_ClearPendingIRQ(UART2_IRQn);
    NVIC_EnableIRQ(UART2_IRQn);

    UART2->C2 &= ((UART_C2_TIE_MASK) | (UART_C2_RIE_MASK));
    UART2->C2 |= ((UART_C2_RIE_MASK));

    // Enable transmitter and Receiver
    UART2->C2 |= (UART_C2_RE_MASK);
}

void UART2_IRQHandler(void) {
    NVIC_ClearPendingIRQ(UART2_IRQn);

    // Receive operation activated
    if (UART2->S1 & UART_S1_RDRF_MASK) {
        if (!Q_Full(&rxQ))
            Q_Enqueue(&rxQ, UART2->D);
    }
}
