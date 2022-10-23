#ifndef LED_H
#define LED_H

#include <Arduino.h>
#include <FastLED.h>
#include "defines.h"

extern CRGB leds[NUM_LEDS];
extern CRGB solidColor;
CRGB parseColor(String value);

#endif