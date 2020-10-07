#define PTB0_Pin 0
#define PTB1_Pin 1
#define PTB2_Pin 2
#define PTB3_Pin 3
#define PTD0_Pin 0
#define PTD2_Pin 2
#define PTD3_Pin 3
#define PTD5_Pin 5

// Initializing Pwm to motors
void initPWM(void);

// Movement functions
void stop(void);
void moveN(void);
void moveNE(void);
void moveE(void);
void moveSE(void);
void moveS(void);
void moveSW(void);
void moveW(void);
void moveNW(void);
