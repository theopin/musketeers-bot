# include <MKL25Z4.h>
# include "myCircularBuffer.h"

# define BAUD_RATE 9600
# define UART_TX_PTE22 22
# define UART_RX_PTE23 23
# define UART2_INT_PRIO 128

volatile int count = 0;

// Interrupt Method
void initUART2Interrupt(uint32_t baud_rate) {
    
    Q_Init(&txQ);
    Q_Init(&rxQ);
    
    NVIC_SetPriority(UART2_IRQn, 128);
    NVIC_ClearPendingIRQ(UART2_IRQn);
    NVIC_EnableIRQ(UART2_IRQn);
    
    UART2->C2 &= ((UART_C2_TIE_MASK) | (UART_C2_RIE_MASK));
    UART2->C2 |= ((UART_C2_TIE_MASK) | (UART_C2_RIE_MASK));
}

void initUART2(uint32_t baud_rate) {
    uint32_t divisor, busClock;
    
    // Enable clock to Port E and UART 2
    SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
    
    // 4 - Alt 4 Location of UART2 -TX/RX
    PORTE->PCR[UART_TX_PTE22] &= ~PORT_PCR_MUX_MASK;
    PORTE->PCR[UART_TX_PTE22] |= PORT_PCR_MUX(4);
    
    PORTE->PCR[UART_RX_PTE23] &= ~PORT_PCR_MUX_MASK;
    PORTE->PCR[UART_RX_PTE23] |= PORT_PCR_MUX(4);    
    
    
    // Disable transmitter and Receiver
    UART2->C2 &= ~((UART_C2_TE_MASK) | (UART_C2_RE_MASK));
   
   /* 
    * busClock: UART runs at half the rate of systemClock
    * divisor: Scale to divide by - include oversampling
    * UART_BDH_SBR; Upper bits of the divisor
    * UART_BDL_SBR; Lower bits of the divisor    
    */
    busClock = (DEFAULT_SYSTEM_CLOCK)/2;
    divisor = busClock / (baud_rate * 16);
    UART2->BDH = UART_BDH_SBR(divisor >> 8);
    UART2->BDL = UART_BDL_SBR(divisor);
    
    // No parity, 8 bits, two stop bits, other settings;
    UART2->C1 = 0;
    UART2->S2 = 0;
    UART2->C3 = 0;
    
    initUART2Interrupt(BAUD_RATE);
    
    // Enable transmitter and Receiver
    UART2->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK);
}


void enqueueTXData(uint8_t dataSend) {
    int opResult;
       
    if (!(UART2->C2 & UART_C2_TIE_MASK))
        UART2->C2 |= UART_C2_TIE_MASK;
    do
        opResult = Q_Enqueue(&txQ, dataSend);
    while(!opResult);   
    
}



void UART2_IRQHandler(void) {  
    NVIC_ClearPendingIRQ(UART2_IRQn);
    
    // Transmit operation activated
    if (UART2->S1 & UART_S1_TDRE_MASK) {
        
        // Character can be sent to txQ  
        if (!Q_Empty(&txQ)){
            UART2->D = Q_Dequeue(&txQ);
            count++;
        }
        // Queue Empty - disable tx interrupt temp
        else 
            UART2->C2 &= ~UART_C2_TIE_MASK;
        
    }
   
    // Receive operation activated
    if (UART2->S1 & UART_S1_RDRF_MASK) {     
        // queue is not full
        if (!Q_Full(&rxQ)) 
            Q_Enqueue(&rxQ, UART2->D);
        
        //else  
            // transmit queuefull message?
            //while(1);
       
    }
}
/*
    // Error Detected
    if (UART2->S1 & (UART_S1_OR_MASK |
                    UART_S1_NF_MASK |
                    UART_S1_FE_MASK |
                    UART_S1_PF_MASK)) {
        // handle the error
        // clear the flag
    }

}

*/
