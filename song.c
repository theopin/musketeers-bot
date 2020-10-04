#include "MKL25Z4.h"                    // Device header

// PROBLEMS PAUSE PLAYING THE SOUND

#define MASK(x) (1 << x)
#define PTB0_Pin 0
#define PTB1_Pin 1
#define SONG_BPM 4000
#define PIT_CHANNEL 0
#define MIN_BUZZER_VOL 0.005
#define FULL_BUZZER_VOL 0.5
#define BUZZER_VOL_DECAY .955

typedef enum {PRIORITY_HIGH = 0, PRIORITY_MED = 1, PRIORITY_MED_LOW = 2, PRIORITY_LOW = 3} interrupt_priorities;
typedef enum {FALLING = 0x0a, RISING = 0x09, TOGGLE = 0x0b} interrupt_triggers;
typedef enum {PAUSE = 0, E4 = 329, F4 = 349, G4 = 392, A4 = 440, AS4 = 466, B4 = 494, C5 = 523 , D5 = 587 , E5 = 659 , F5 = 698, FS5 = 740, G5 = 783 , AF5 = 830, A5 = 880 , B5 = 987, C6 = 1047, E6 = 1319} music_notes_freq;
typedef enum {HOW_YOU_LIKE_THAT, SUCCESS} songs;
typedef enum {false, true} bool;

static int success_fx_notes[] = {
    F4, G4, AS4
};
static int success_fx_beats[] = { 
    4, 4, 8
};

static int how_you_like_that_notes[] = {
E6, B4, C5, A4,
B4, G4, A4, F4,
E4, G4, F4, A4,
G4, B4, A4, D5,
E5,
PAUSE, E5, E5, FS5,
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
static int how_you_like_that_beats[] = {
8, 8, 8, 8,
8, 8, 8, 8,
8, 8, 8, 8,
8, 8, 8, 8,
48,
48, 4, 4, 4,
16, 4, 4, 4, 4,
16, 4, 4, 4, 4,
8, 4, 4, 4, 4, 8,
4, 4, 8, 4, 4, 4, 4,
16, 4, 4, 4, 4,
16, 4, 4, 4, 4,
8, 4, 4, 4, 4, 8,
4, 4, 8, 8, 4, 4,
6, 6, 4, 6, 6, 4,
6, 6, 4, 12 , 4, //F # F#
6, 6, 4, 6, 6, 4,
6, 6, 4, 4, 4, 8,
6, 6, 4, 6, 6, 4,
6, 6, 4, 8, 2, 6,
2, 6, 2, 6, 8, 8,
8, 8, 4, 4, 8,
8, 4, 4, 8, 8,
4, 4, 8, 2, 6, 8, // AAA the last 3 notes
8, 4, 4, 8, 8,
4, 4, 8, 4, 4, 8,
8, 4, 4, 8, 8,
4, 4, 8, 4, 4, 8,
8, 4, 4, 8, 8,
4, 4, 8, 8, 8,
8, 2, 2, 8, 4, 2 , 2, 4, // lyrics :look at you look at me
8, 2, 2, 8, 4, 2 , 2, 4,
8, 2, 2, 8, 4, 2 , 2, 4,
4, 4, 8, 8 , 8,
8, 2, 2, 8, 4, 2 , 2, 4,
8, 2, 2, 8, 4, 2 , 2, 4,
8, 2, 2, 8, 4, 2 , 2, 4,
4, 4, 8, 16,
};

int a;
static int* cur_song_notes;
static int* cur_song_beats;
static unsigned int cur_song_size = 0;
static volatile int is_song_playing = false;
static volatile unsigned int note_counter = 0;  
static volatile int beats_to_next_note = 0;
static double buzzer_volume = .5;

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

void playNote(int note){
    // end of song
    if (note_counter == cur_song_size -1) { 
        // selects no clock to TPM which disables it
        SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
        return;
    }
 
    if (note != PAUSE){
        changeBuzzerFrequency(note);
        changeBuzzerVolume(FULL_BUZZER_VOL); // resets the volume from the fade out
    }
    beats_to_next_note = cur_song_beats[note_counter];
    
    note_counter += 1;
} 

void PIT_IRQHandler(){
    // Clear Pending IRQ from PIT
    NVIC_ClearPendingIRQ(PIT_IRQn);
    
    // Fade out effect for current note played
    buzzer_volume = max(buzzer_volume, MIN_BUZZER_VOL);
    changeBuzzerVolume(buzzer_volume * buzzer_volume* BUZZER_VOL_DECAY);
    
    beats_to_next_note -= 1;
    // if the PIT channel is the one interrpupted
    if (PIT->CHANNEL[PIT_CHANNEL].TFLG & PIT_TFLG_TIF(1) && beats_to_next_note <= 0){
        playNote(cur_song_notes[note_counter]);
    }
    
    //Clear INT Flag
    PIT->CHANNEL[PIT_CHANNEL].TFLG |= PIT_TFLG_TIF(1);
}

void initPIT(){
    // enable clock to PIT
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;     
    
    // set load value the timer resets to, used to set BPM of song
    PIT->CHANNEL[PIT_CHANNEL].LDVAL = bpmToPitLoadVal(SONG_BPM);
    
    // enable that PIT CH0 IRQ
    NVIC_SetPriority(PIT_IRQn, PRIORITY_MED);
    NVIC_ClearPendingIRQ(PIT_IRQn);
    NVIC_EnableIRQ(PIT_IRQn);
    
    // clears the interrupt flag so that isr is not immediately called if there were pending interrupts.
    PIT->CHANNEL[PIT_CHANNEL].TFLG |= PIT_TFLG_TIF(1);
    // enable PIT module timer and interrupts for that specific channel
    PIT->CHANNEL[PIT_CHANNEL].TCTRL |= (PIT_TCTRL_TEN_MASK | PIT_TCTRL_TIE_MASK);
}

void initPWM(){
    // Enable Clock for portB. This starts the port
    SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
    
    // Set pins to TPM1_CH0 and TMP1_CH1 mode to their ALT 3 configs (act as a timer) respectively
    PORTB->PCR[PTB0_Pin] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[PTB0_Pin] |= PORT_PCR_MUX(3);
    
    PORTB->PCR[PTB1_Pin] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[PTB1_Pin] |= PORT_PCR_MUX(3);
    
    // enables the TMP1 Clock
    SIM_SCGC6 |= SIM_SCGC6_TPM1_MASK;
    
    // enable PIT module
    PIT->MCR &= ~PIT_MCR_MDIS_MASK;
    
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

void playSound(int num){
    // sets the current song
    switch(num){
        case HOW_YOU_LIKE_THAT: {
            cur_song_notes = how_you_like_that_notes;
            cur_song_beats = how_you_like_that_beats;
            cur_song_size = sizeof(how_you_like_that_notes) / sizeof (int);
            break;
        } case SUCCESS: {
            cur_song_notes = success_fx_notes;
            cur_song_beats = success_fx_beats;
            cur_song_size = sizeof(success_fx_notes) / sizeof (int);
            break;
        } default: {
            break;
        }
    }
       
    // start the counter
    note_counter = 0;
    beats_to_next_note = 0;
    
    
    // Selects clock to TPM module
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
}

void playSong(){
    playSound(HOW_YOU_LIKE_THAT);
   
}
void playSuccessFx(){
    playSound(SUCCESS);
}

static void delay(volatile uint32_t nof) {
    while (nof != 0) {
        __asm("NOP");
        nof--;
    }
}

int main(){
    initPIT();
    initPWM();
    playSong();
    while(1){
    }
}

