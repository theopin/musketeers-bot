#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1
#define NUM_LEDS 3

typedef enum {
    RED = RED_LED, 
    GREEN = GREEN_LED, 
    BLUE = BLUE_LED
} led_colors_t;

void initInternalLED(void);
void toggleLED(led_colors_t color);


