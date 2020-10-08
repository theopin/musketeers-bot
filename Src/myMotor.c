#include "myMotor.h"
#include "MKL25Z4.h"                    // Device header

void configurePWMModeForPortB(int pin) {
    PORTB->PCR[pin] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[pin] |= PORT_PCR_MUX(3);
}

void configurePWMModeForPortD(int pin) {
    PORTD->PCR[pin] &= ~PORT_PCR_MUX_MASK;
    PORTD->PCR[pin] |= PORT_PCR_MUX(4);
}

void setEdgedAlignedPWMAngHighTruePulse(__IO uint32_t *timer) {
    *timer &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
    *timer |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
}

void setCountModeAndPreScalar(__IO uint32_t *timer) {
    *timer &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
    *timer |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
    *timer &= ~(TPM_SC_CPWMS_MASK);
}

/* initPWM() */
void initPWM(void) {
    // Enable clock gating for port b and d
    SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
    SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;

    // Configure PWM mode - 3 for PortB, 4 for PortD
    configurePWMModeForPortB(PTB0_Pin);
    configurePWMModeForPortB(PTB1_Pin);
    configurePWMModeForPortB(PTB2_Pin);
    configurePWMModeForPortB(PTB3_Pin);
    configurePWMModeForPortD(PTD0_Pin);
    configurePWMModeForPortD(PTD2_Pin);
    configurePWMModeForPortD(PTD3_Pin);
    configurePWMModeForPortD(PTD5_Pin);

    // Enable clock gating for timer 0, 1, 2
    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
    SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
    SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;

    // Select clock source for TPM (MCGFLLCLK / MCGGPLLCLK/2)
    SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

    //Set Frequency
    TPM0->MOD = 100;
    TPM1->MOD = 100;
    TPM2->MOD = 100;
    // Update status and control.
    // CMOD: 01 (LPTM counter increments on every count)
    // PS: 111 (Prescalar of 128)
    setCountModeAndPreScalar(&(TPM0->SC));
    setCountModeAndPreScalar(&(TPM1->SC));
    setCountModeAndPreScalar(&(TPM2->SC));
    //Edge-aligned PWM with high true pulse
    setEdgedAlignedPWMAngHighTruePulse(&(TPM0_C0SC));
    setEdgedAlignedPWMAngHighTruePulse(&(TPM0_C2SC));
    setEdgedAlignedPWMAngHighTruePulse(&(TPM0_C3SC));
    setEdgedAlignedPWMAngHighTruePulse(&(TPM0_C5SC));
    setEdgedAlignedPWMAngHighTruePulse(&(TPM1_C0SC));
    setEdgedAlignedPWMAngHighTruePulse(&(TPM1_C1SC));
    setEdgedAlignedPWMAngHighTruePulse(&(TPM2_C0SC));
    setEdgedAlignedPWMAngHighTruePulse(&(TPM2_C1SC));
}

//	                   LEFT	       LEFT	           LEFT	         LEFT	     RIGHT	      RIGHT	        RIGHT	   RIGHT
void changeAllFourPWM(double T1C0, double T1C1, double T2C0, double T2C1, double T0C0, double T0C2, double T0C3, double T0C5) {
    double fr_multiplier = 75;
    double fl_multiplier = 70;
    double br_multiplier = 75;
    double bl_multiplier = 70;
    TPM1_C0V = (int)(T1C0 * fr_multiplier);
    TPM1_C1V = (int)(T1C1 * fr_multiplier);
    TPM2_C0V = (int)(T2C0 * br_multiplier);
    TPM2_C1V = (int)(T2C1 * br_multiplier);
    TPM0_C0V = (int)(T0C0 * bl_multiplier);
    TPM0_C2V = (int)(T0C2 * fl_multiplier);
    TPM0_C3V = (int)(T0C3 * fl_multiplier);
    TPM0_C5V = (int)(T0C5 * bl_multiplier);
}

void stop(void) {
    changeAllFourPWM(0, 0, 0, 0, 0, 0, 0, 0);
}

void moveN(void) {
    changeAllFourPWM(0, 1, 0, 1, 0, 1, 0, 1);
}

void moveNE(void) {
    changeAllFourPWM(0, 1, 0, 1, 0, 0.55, 0, 0.55);
}

void moveE(void) {
    changeAllFourPWM(0, 1, 0, 1, 0, 0, 0, 0);
}

void moveSE(void) {
    changeAllFourPWM(0, 0, 0, 0, 0, 0, 0, 0);
}

void moveS(void) {
    changeAllFourPWM(1, 0, 1, 0, 1, 0, 1, 0);
}

void moveSW(void) {
    changeAllFourPWM(0, 1, 0, 1, 0, 0, 0, 0);
}

void moveW(void) {
    changeAllFourPWM(0, 0, 0, 0, 0, 1, 0, 1);
}

void moveNW(void) {
    changeAllFourPWM(0, 0.55, 0, 0.55, 0, 1, 0, 1);
}

int set_pwm_fl_multipler(int message) {
    int fl_multiplier = message - 256;
    return fl_multiplier;
}

int set_pwm_fr_multipler(int message) {
    int fr_multiplier = message - 512;
    return fr_multiplier;
}

int set_pwm_bl_multipler(int message) {
    int bl_multiplier = message - 1024;
    return bl_multiplier;
}

int set_pwm_br_multipler(int message) {
    int br_multiplier = message - 2048;
    return br_multiplier;
}
