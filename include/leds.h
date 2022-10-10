#ifndef LED_H
#define LED_H

#include <Arduino.h>
#include <FastLED.h>
#include "defines.h"

uint8_t g_lineHeight = 0;
uint8_t g_Brightness = 128;
uint8_t g_Power = 1;
uint8_t currentPatternIndex = 0;

CRGB leds[NUM_LEDS] = {0};
CRGB solidColor = CRGB::Red;
CRGB parseColor(String value);

#endif