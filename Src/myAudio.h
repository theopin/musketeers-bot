#include "myCommonFunctions.h"

#define PTC8 8
#define PIT_CHANNEL 0
#define SONG_PLAYBACK_RATE 2000
#define MIN_BUZZER_VOL 0.005
#define FULL_BUZZER_VOL 0.1
#define BUZZER_VOL_DECAY .935

typedef enum {
    PRIORITY_HIGH = 0,
    PRIORITY_MED = 1,
    PRIORITY_MED_LOW = 2,
    PRIORITY_LOW = 3
} interrupt_priorities;

typedef enum {
    FALLING = 0x0a,
    RISING = 0x09,
    TOGGLE = 0x0b
} interrupt_triggers;

typedef enum {
    PAUSE = 0,
    E4 = 329,
    F4 = 349,
    G4 = 392,
    A4 = 440,
    AS4 = 466,
    B4 = 494,
    C5 = 523,
    D5 = 587 ,
    DS5= 622,
    E5 = 659 ,
    F5 = 698,
    FS5 = 740, 
    G5 = 783 ,
    AF5 = 830,
    A5 = 880 ,
    B5 = 987,
    C6 = 1047,
    E6 = 1319
} music_notes_freq;

typedef enum {
    HOW_YOU_LIKE_THAT,
    SUCCESS_FX,
    STOP_SONG,
    VICTORY_TUNE
} songs;

typedef enum {  
    false,
    true
} bool_t;

void initAudio(void);
void stopSong(void);
void startSuccessFx(void);
void startSong(void);
void startVictoryTune(void);
