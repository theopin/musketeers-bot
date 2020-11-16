#include "myAudio.h"
#include "MKL25Z4.h"

static int stop_song_notes[] = {
    PAUSE
};
static int stop_song_beats[] = { 
    0
};  

static int victory_notes[] = {
    AS5, F5A, F5,
    AS5, B5, C6, G5A, G5,
    C6,PAUSE, D6, PAUSE, D6
     
};
static int victory_beats[] = { 
    6, 2, 12,
    4, 4, 6, 2, 12,
    4, 4, 4, 8, 2
};

static int success_fx_notes[] = {
    F4, G4, AS4, PAUSE
};
static int success_fx_beats[] = { 
    4, 4, 8, 32 
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
            SIM->SCGC6 &= ~SIM_SCGC6_TPM2_MASK; // Disables it TPM modules
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
        case STOP_SONG:
            cur_song_notes = stop_song_notes;
            cur_song_beats = stop_song_beats;
            cur_song_size = sizeof(stop_song_notes) / sizeof (int);
            break;
        case VICTORY_TUNE:
            cur_song_notes = victory_notes;
            cur_song_beats = victory_beats;
            cur_song_size = sizeof(victory_notes) / sizeof (int);
            break;
        default:
            break;
    }
       
    // start the counter
    note_counter = 0;
    beats_to_next_note = 0;
    
    // Selects clock to TPM module
    SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;
}

void startSong() {
    is_sound_looping = true;
    startSound(HOW_YOU_LIKE_THAT);
}

void stopSong() {
    is_sound_looping = false;
    startSound(STOP_SONG);
}

void startSuccessFx() {
    is_sound_looping = false;
    startSound(SUCCESS_FX);
}

void startVictoryTune() {
    is_sound_looping = false;
    startSound(VICTORY_TUNE);
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
}   
