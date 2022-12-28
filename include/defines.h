#ifndef DEFINES_H
#define DEFINES_H

/*****************
DEFINITIONS
******************/
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
#define TIMES_PER_SECOND(x) EVERY_N_MILLISECONDS(1000 / x)
#define FASTLED_INTERNAL
#define EEPROM_SIZE 12

#define LED_BUILTIN 2

#define POWER_BUTTON 4
#define PATTERN_BUTTON 15
#define BRIGHTNESS_INC 19
#define BRIGHTNESS_DEC 18
#define RESET_BUTTON

#define OLED_CLOCK 22
#define OLED_DATA 21
#define OLED_RESET 16

#define NUM_STRIPS 2
#define LEDS_STRIP_1 74
#define LEDS_STRIP_2 72
#define NUM_LEDS LEDS_STRIP_1
#define LED_PIN_STRIP_1 5
#define LED_PIN_STRIP_2 17
#define COLOR_SEQUENCE GRB
#define CHIPSET WS2812B
#define MILLI_AMPS 4000 // IMPORTANT: set the max milli-Amps of your power supply (4A = 4000mA)

#define FRAMES_PER_SECOND 120
#define BRIGHTNESS_INCREMENT 16

// EEPROM addresses for state
#define SSID_INDEX 1
#define PASSWORD_INDEX 2
#define WIFI_SET 3
#define MDNS_INDEX 4
#define MDNS_SET 5
#define AP_SET 6

extern uint8_t g_Brightness;
extern uint8_t g_Power;
extern uint8_t currentPatternIndex;

#endif