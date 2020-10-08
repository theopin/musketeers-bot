#define CONNECTION_FLASH_DELAY 250000
#define CONNECTION_REPS_DELAY 250000

#define GREEN_RUN_DELAY 750000
#define RED_RUN_DELAY 500000
#define RED_STOP_DELAY 250000
#define NUM_GREEN_LEDS 10

#define PTA17 17
typedef enum {
    PTC3 = 3,
    PTC4 = 4,
    PTC5 = 5,
    PTC6 = 6,
    PTC10 = 10,
    PTC11 = 11,
    PTC12 = 12,
    PTC13 = 13,
    PTC16 = 16,
    PTC17 = 17
} port_c_t;

void initExternalLED();
void toggleRedLED();
void onGreenLED(int led);
void offGreenLED(int led);
void clearLitGreenLED();
void nextTrailGreenLED();
void setAllGreenLED();
void signalSuccessConnection();
void runStationeryLED();
void runMotionLED();
void runExternalLED();