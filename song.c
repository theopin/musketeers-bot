#include "MKL25Z4.h"                    // Device header

#define MASK(x) (1 << x)
#define PTB0_Pin 0
#define PTB1_Pin 1
#define BPM 4000
#define PIT_CHANNEL 0
#define NORMAL_BUZZER_VOL 0.5
#define MIN_BUZZER_VOL 0.005
#define BUZZER_VOL_DECAY .955

typedef enum {PRIORITY_HIGH = 0, PRIORITY_MED = 1, PRIORITY_MED_LOW = 2, PRIORITY_LOW = 3} interrupt_priorities;
typedef enum {FALLING = 0x0a, RISING = 0x09, TOGGLE = 0x0b} interrupt_triggers;
typedef enum {PAUSE = 10, E4 = 329, F4 = 349, G4 = 392, A4 = 440, AS4 = 466, B4 = 494, C5 = 523 , D5 = 587 , E5 = 659 , F5 = 698, FS5 = 740, G5 = 783 , AF5 = 830, A5 = 880 , B5 = 987, C6 = 1047, E6 = 1319} music_notes_freq;

int song_notes[] = {
    E6, B4, C5, A4,
    B4, G4, A4, F4,
    E4, G4, F4, A4,
    G4, B4, A4, D5,
    E5,
    E5, PAUSE, E5, E5, FS5,
    G5, PAUSE, FS5, G5, FS5,
    D5, PAUSE, D5, D5, D5, 
    B5, B5, B5, B5, A5, G5,
    A5, G5, E5, PAUSE, E5, E5, FS5,
    G5, PAUSE, FS5, G5, FS5,
    D5, PAUSE, D5, D5, D5,
    B5, B5, B5, B5, A5, G5,
    A5, G5, E5, PAUSE, E5, FS5,
    G5, G5, E5, G5, G5, E5,
    FS5, FS5, D5, D5, D5,
    B5, B5, B5, B5, A5, G5,
    C6, C6, C6, B5, A5, G5,
    G5, G5, E5, G5, G5, E5,
    FS5, FS5, D5, D5, D5, D5,
    G5, G5, A5, A5, B5, A5,
    PAUSE, E5, E5, E5, E5,
    E6, PAUSE, B4, B4, E5,
    F5, C5, F5, A5, A5, A5,
    B5, G5, FS5, E5, E5,
    F5, C5, F5, E5, E5, E5,
    E6, PAUSE, B4, B4, E5,
    F5, C5, F5, A5, A5, A5,
    B5, G5, FS5, E5, E5,
    F5, C5, F5, E5, E5,
    PAUSE, G5, G5, G5, FS5, G5, FS5, E5,
    E5, G5, G5, G5, FS5, G5, FS5, E5,
    E5, G5, G5, G5, FS5, G5, FS5, E5,
    F5, C5, F5, E5, E5,
    PAUSE, G5, G5, G5, FS5, G5, FS5, E5,
    E5, G5, G5, G5, FS5, G5, FS5, E5,
    E5, G5, G5, G5, FS5, G5, FS5, E5,
    F5, C5, F5, E5


};
int song_beats[] = {
    8, 8, 8, 8,
    8, 8, 8, 8,
    8, 8, 8, 8,
    8, 8, 8, 8,
    32,
    16, 4, 4, 4, 4,
    16, 4, 4, 4, 4,
    16, 4, 4, 4, 4,
    8, 4, 4, 4, 4, 8,
    4, 4, 8, 4, 4, 4, 4,
    16, 4, 4, 4, 4,
    16, 4, 4, 4, 4,
    8, 4, 4, 4, 4, 8,
    4, 4, 8, 8, 4, 4,
    6, 6, 4, 6, 6, 4,
    6, 6, 4, 12, 4,//F# F#
    6, 6, 4, 6, 6, 4,
    6, 6, 4, 4, 4, 8,
    6, 6, 4, 6, 6, 4,
    6, 6, 4, 8, 2, 6,
    2, 6, 2, 6, 8, 8,
    8, 8, 4, 4, 8,
    8, 4, 4, 8, 8,
    4, 4, 8, 2, 6, 8, //AAA AT THE END
    8, 4, 4, 8, 8,
    4, 4, 8, 4, 4, 8,
    8, 4, 4, 8, 8,
    4, 4, 8, 4, 4, 8,
    8, 4, 4, 8, 8,
    4, 4, 8, 8, 8,
    8, 2, 2, 8, 4, 2 , 2, 4, // FIRST LOOK AT YOU LOOK AT ME
    8, 2, 2, 8, 4, 2 , 2, 4,
    8, 2, 2, 8, 4, 2 , 2, 4,
    4, 4, 8, 8 , 8,
    8, 2, 2, 8, 4, 2 , 2, 4,
    8, 2, 2, 8, 4, 2 , 2, 4,
    8, 2, 2, 8, 4, 2 , 2, 4,
    4, 4, 8, 16,

};

unsigned int song_size = 0;
unsigned int music_score_counter = 0;  
int beats_to_next_note = 0;
double buzzer_volume = .5;

double max(double val1, double val2){
    if (val1 > val2){
        return val1;
    } else {
        return val2;
    }
}
void changeBuzzerFrequency(int freq){
    TPM1->MOD = 375000/freq - 1; 
}

int bpmToPitLoadVal(int beats_per_minute) {
    return 0xABA95000/beats_per_minute;
}

void changeBuzzerVolume(double target_volume){
    TPM1_C0V =  TPM1->MOD * target_volume;
    buzzer_volume = target_volume;
};

void playNextNote(){
    music_score_counter += 1;
    music_score_counter %= song_size;

    int note = song_notes[music_score_counter];
    if (note != PAUSE){
        changeBuzzerFrequency(song_notes[music_score_counter]);
        changeBuzzerVolume(buzzer_volume);
    }
    beats_to_next_note = song_beats[music_score_counter];
} 

void PIT_IRQHandler(){
    // Clear Pending IRQ
    NVIC_ClearPendingIRQ(PORTD_IRQn);

    // Fade out effect
    buzzer_volume = max(buzzer_volume, MIN_BUZZER_VOL);
    changeBuzzerVolume(buzzer_volume * BUZZER_VOL_DECAY * BUZZER_VOL_DECAY);

    beats_to_next_note -= 1;
    if (PIT->CHANNEL[PIT_CHANNEL].TFLG & PIT_TFLG_TIF(1) && beats_to_next_note <= 0){
        changeBuzzerVolume(NORMAL_BUZZER_VOL);
        playNextNote();
    }

    //Clear INT Flag
    PIT->CHANNEL[PIT_CHANNEL].TFLG |= PIT_TFLG_TIF(1);
}
void initSong() {
    song_size = sizeof(song_notes)/sizeof(song_notes[0]);
}
void initPIT(){
    // enable clock to PIT
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;     

    // enable PIT module
    PIT->MCR &= ~PIT_MCR_MDIS_MASK;

    // set load value the timer resets to, used to set BPM of song
    PIT->CHANNEL[PIT_CHANNEL].LDVAL = bpmToPitLoadVal(BPM);

    // enable that PIT CH0 IRQ
    NVIC_SetPriority(PIT_IRQn, PRIORITY_MED_LOW);
    NVIC_ClearPendingIRQ(PIT_IRQn);
    NVIC_EnableIRQ(PIT_IRQn);

    // clears the interrupt flag so that isr is not immediately called if there were pending interrupts.
    PIT->CHANNEL[PIT_CHANNEL].TFLG = PIT_TFLG_TIF(0);
    // enable PIT module timer and interrupts for that specific channel
    PIT->CHANNEL[PIT_CHANNEL].TCTRL |= (PIT_TCTRL_TEN_MASK | PIT_TCTRL_TIE_MASK);
}

void initPWM(){
    // Enable Clock for portB. This starts the port
    SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;

    // Set pins to TPM1_CH0 and TMP1_CH1 mode respectively (their ALT 3 configs, act as a timer)
    PORTB->PCR[PTB0_Pin] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[PTB0_Pin] |= PORT_PCR_MUX(3);

    PORTB->PCR[PTB1_Pin] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[PTB1_Pin] |= PORT_PCR_MUX(3);

    // enables the TMP1 Clock
    SIM_SCGC6 |= SIM_SCGC6_TPM1_MASK;

    // Selects the Clock source for the TPM pins to be OSCERCLK, an internal oscillator
    SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

    // MOD is 16 bit val up to which the counter counts, at which it resets to 0
    // 50 Hz PWM 	
    TPM1->MOD = 7500;
    TPM1_C0V = 3400;

    // cmod is about internal (use this) or external clock
    // ps is prescale value of 128  
    TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
    TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));

    // count up
    TPM1->SC &= ~(TPM_SC_CPWMS_MASK);

    //  Edge-aligned PWM MODE High-true pulses (clear Output on match, set Output on reload)
    TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
    TPM1_C0SC |= (TPM_CnSC_ELSB(1)) | (TPM_CnSC_MSB(1));

    TPM1_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
    TPM1_C1SC |= (TPM_CnSC_ELSB(1)) | (TPM_CnSC_MSB(1));   
}

int main(){
    initSong();
    initPIT();
    initPWM();
    while(1){
    }
}
