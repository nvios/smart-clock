#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include "Config.h"
#include <Adafruit_NeoPixel.h>

extern Adafruit_NeoPixel ring;

enum class LEDState
{
    IDLE,
    EVENT_RAINBOW,
    VOC_GREEN,
    VOC_YELLOW,
    VOC_RED,
    VOC_RED_PULSE
};

void initLED();
void updateLED();
void setLEDState(LEDState newState);

#endif