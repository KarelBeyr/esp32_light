#ifndef Led_h
#define Led_h
// use first channel of 16 channels (started from zero)
#define LEDC_CHANNEL_0 0

// use 13 bit precission for LEDC timer
#define LEDC_TIMER_13_BIT 13

#define LED_PIN 2 

void changeLedPwm(int duty, int freq, bool automaticMode, int measuredLux, int desiredLux, float utlumStin, float dutyFor100lux);
#endif