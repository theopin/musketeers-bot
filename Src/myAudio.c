#include "myAudio.h"
#include "MKL25Z4.h"

static int success_fx_notes[] = {
    F4, G4, AS4
};
static int success_fx_beats[] = { 
    4, 4, 8
};

static int how_you_like_that_notes[] = {
B4, C5, A4,
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
8, 8, 8,
8, 8, 8, 8,
8, 8, 8, 8,
8, 8, 8, 8,
32,
4, 4, 4, 4,
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

static int is_sound_looping = false;
static int* cur_song_notes;
static int* cur_song_beats;
static unsigned int cur_song_size = 0;
static unsigned int note_counter = 0;  
static int beats_to_next_note = 0;
static double buzzer_volume = .5;
    
int bpmToPITLoadVal(int beats_per_minute) {
    return 0xABA95000/beats_per_minute;
}

void changeBuzzerFrequency(int freq) {
      TPM0->MOD = 375000/freq - 1; 
}
  
void changeBuzzerVolume(double target_volume) {
    TPM0_C0V =  TPM0->MOD * target_volume;
    buzzer_volume = target_volume;
}

void playNote(int note) {
    // end of song
    if (note_counter == cur_song_size -1) {
        if (is_sound_looping) {
            note_counter = 0;
        } else {
            SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK; // Disables it TPM modules
        return;
        }
    }
 
    if (note != PAUSE) {
        changeBuzzerFrequency(note);
        changeBuzzerVolume(FULL_BUZZER_VOL); // resets the volume from the fade out
    }
    beats_to_next_note = cur_song_beats[note_counter];
    
    note_counter += 1;
} 

void fadeVolume() {
    buzzer_volume = max(buzzer_volume * BUZZER_VOL_DECAY, MIN_BUZZER_VOL);
    changeBuzzerVolume(buzzer_volume);
}
    
void PIT_IRQHandler() {
    // Clear Pending IRQ from PIT
    NVIC_ClearPendingIRQ(PIT_IRQn);
    
    fadeVolume();
    
    beats_to_next_note -= 1;
    // if the PIT channel is the one interrpupted
    if (PIT->CHANNEL[PIT_CHANNEL].TFLG & PIT_TFLG_TIF(1) && beats_to_next_note <= 0) {
        playNote(cur_song_notes[note_counter]);
    }
    
    //Clear INT Flag
    PIT->CHANNEL[PIT_CHANNEL].TFLG |= PIT_TFLG_TIF(1);
}

void startSound(int sound_id) {
    // Sets the current sound playing
    switch(sound_id) {
        case HOW_YOU_LIKE_THAT:
            cur_song_notes = how_you_like_that_notes;
            cur_song_beats = how_you_like_that_beats;
            cur_song_size = sizeof(how_you_like_that_notes) / sizeof (int);
            break;
        case SUCCESS_FX:
            cur_song_notes = success_fx_notes;
            cur_song_beats = success_fx_beats;
            cur_song_size = sizeof(success_fx_notes) / sizeof (int);
            break;
        default:
            break;
    }
       
    // start the counter
    note_counter = 0;
    beats_to_next_note = 0;
    
    // Selects clock to TPM module
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
}

void startSong() {
    is_sound_looping = true;
    startSound(HOW_YOU_LIKE_THAT);
}

void startSuccessFx() {
    is_sound_looping = false;
    startSound(SUCCESS_FX);
}

void initAudioPWM() {
    // Enable Clock for portC. This starts the port
    SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
    
    // Set multiplexing of pins to TPM0_CH0 config
    PORTC->PCR[PTC1] &= ~PORT_PCR_MUX_MASK;
    PORTC->PCR[PTC1] |= PORT_PCR_MUX(4);
    
    // enables the TMP0 Clock
    SIM_SCGC6 |= SIM_SCGC6_TPM0_MASK;
    
    // Selects the Clock source for the TPM pins to be OSCERCLK, an internal oscillator
    SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
    
    // MOD is 16 bit val up to which the counter counts, at which it resets to 0
    // Note how all channels under TPM0 use the same MOD.
    // CNV is used as a match value when outputting.
    TPM0->MOD = 7500;
    TPM0_C0V = 3400;
    
    // Cmod controls use of internal (use this) or external clock
    // PS is a prescaler value of 128  
    TPM0->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
    TPM0->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
    
    // Count up
    TPM0->SC &= ~(TPM_SC_CPWMS_MASK);
    
    //  Edge-aligned PWM MODE High-true pulses (clear Output on match, set Output on reload)
    TPM0_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
    TPM0_C0SC |= (TPM_CnSC_ELSB(1)) | (TPM_CnSC_MSB(1));
}


void initAudioPIT() {
    // Enable clock to PIT
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;     
    
    // Set load value the timer resets to, used to set BPM of song
    PIT->CHANNEL[PIT_CHANNEL].LDVAL = bpmToPITLoadVal(SONG_PLAYBACK_RATE);
    
    // Enable that PIT CH0 IRQ
    NVIC_SetPriority(PIT_IRQn, PRIORITY_MED);
    NVIC_ClearPendingIRQ(PIT_IRQn);
    NVIC_EnableIRQ(PIT_IRQn);
    
    // Clears the interrupt flag so that isr is not immediately called if there were pending interrupts.
    PIT->CHANNEL[PIT_CHANNEL].TFLG |= PIT_TFLG_TIF(1);
    // Enable PIT module timer and interrupts for that specific channel
    PIT->CHANNEL[PIT_CHANNEL].TCTRL |= (PIT_TCTRL_TEN_MASK | PIT_TCTRL_TIE_MASK);
    
    // Enable PIT module
    PIT->MCR &= ~PIT_MCR_MDIS_MASK;
}

void initAudio(){
    initAudioPIT();
    initAudioPWM();
}   
