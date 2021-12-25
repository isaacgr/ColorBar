#include <Arduino.h>
#include <U8g2lib.h>
#include <FastLED.h>
#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include <EEPROM.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3003000)
#warning "Requires FastLED 3.3 or later; check github for latest code."
#endif

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
#define TIMES_PER_SECOND(x) EVERY_N_MILLISECONDS(1000 / x)
#define FASTLED_INTERNAL
#define EEPROM_SIZE 12

#define LED_BUILTIN 2

#define POWER_BUTTON 4
#define PATTERN_BUTTON 15
#define BRIGHTNESS_INC 18
#define BRIGHTNESS_DEC 19

#define OLED_CLOCK 22
#define OLED_DATA 21
#define OLED_RESET 16

#define NUM_STRIPS 2
#define LEDS_PER_STRIP 10
#define NUM_LEDS NUM_STRIPS *LEDS_PER_STRIP
#define LED_PIN_STRIP_1 5
#define LED_PIN_STRIP_2 17
#define COLOR_SEQUENCE GRB
#define CHIPSET WS2812B
#define MILLI_AMPS 1000 // IMPORTANT: set the max milli-Amps of your power supply (4A = 4000mA)

#define FRAMES_PER_SECOND 120

CRGB leds[NUM_LEDS] = {0};

U8G2_SSD1306_128X64_NONAME_F_HW_I2C g_OLED(U8G2_R0, OLED_RESET, OLED_CLOCK, OLED_DATA);
WebServer server(80);

uint8_t g_lineHeight = 0;
uint8_t g_Brightness = 128;
uint8_t g_Power = 1;
int BRIGHTNESS_INCREMENT = 16;

uint8_t currentPatternIndex = 0;
uint8_t currentTemperatureIndex = 0;

const bool apMode = false;

// modifiers for fire, water and pacifica effects
uint8_t g_ColorTemperature = 0;
uint8_t g_ColorPalette = 0;
uint8_t g_Sparking = 100;
uint8_t g_Cooling = 20;
uint8_t g_Sparks = 3;
uint8_t g_SparkHeight = 4;
bool breversed = false;
bool bmirrored = false;
bool g_Cycle = false;
uint8_t g_Speed = 20;

CRGB solidColor = CRGB::Red;

#include <secret.h>
#include <wifi_utils.h>
#include <file_manager.h>
#include <pattern.h>
#include <patterns.h>

#include <field.h>
#include <field_utils.h>
#include <fields.h>
#include <routes.h>

void IRAM_ATTR POWER_ISR()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200)
  {
    g_Power = !g_Power;
  }
  last_interrupt_time = interrupt_time;
}

void IRAM_ATTR PATTERN_ISR()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200)
  {
    if (currentPatternIndex >= patternCount - 1)
    {
      currentPatternIndex = 0;
    }
    else
    {
      currentPatternIndex++;
    }
  }
  last_interrupt_time = interrupt_time;
}

void IRAM_ATTR BRIGHTNESS_INC_ISR()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200)
  {
    if (g_Brightness + BRIGHTNESS_INCREMENT >= 255)
    {
      g_Brightness = 255;
      FastLED.setBrightness(g_Brightness);
    }
    else
    {
      g_Brightness += BRIGHTNESS_INCREMENT;
      FastLED.setBrightness(g_Brightness);
    }
  }
  last_interrupt_time = interrupt_time;
}

void IRAM_ATTR BRIGHTNESS_DEC_ISR()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200)
  {

    if (g_Brightness - BRIGHTNESS_INCREMENT <= 0)
    {
      g_Brightness = 0;
      FastLED.setBrightness(g_Brightness);
    }
    else
    {
      g_Brightness -= BRIGHTNESS_INCREMENT;
      FastLED.setBrightness(g_Brightness);
    }
  }
  last_interrupt_time = interrupt_time;
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_PIN_STRIP_1, OUTPUT);
  pinMode(LED_PIN_STRIP_2, OUTPUT);
  pinMode(POWER_BUTTON, INPUT);
  pinMode(PATTERN_BUTTON, INPUT);
  pinMode(BRIGHTNESS_INC, INPUT);
  pinMode(BRIGHTNESS_DEC, INPUT);

  attachInterrupt(POWER_BUTTON, POWER_ISR, RISING);
  attachInterrupt(PATTERN_BUTTON, PATTERN_ISR, RISING);
  attachInterrupt(BRIGHTNESS_INC, BRIGHTNESS_INC_ISR, RISING);
  attachInterrupt(BRIGHTNESS_DEC, BRIGHTNESS_DEC_ISR, RISING);

  Serial.begin(115200);
  while (!Serial)
  {
  }
  setupMDNS();
  setupWifi();
  // SPIFFS.format(); // Prevents SPIFFS_ERR_NOT_A_FS
  SPIFFS.begin(); // Start the SPI Flash Files System

  server.begin();
  server.enableCORS();
  setupWeb();

  g_OLED.clear();
  g_OLED.begin();
  g_OLED.setFont(u8g2_font_profont12_tf);
  g_lineHeight = g_OLED.getFontAscent() - g_OLED.getFontDescent(); // decent is a negative number, so subtract from the total

  FastLED.addLeds<CHIPSET, LED_PIN_STRIP_1, COLOR_SEQUENCE>(leds, 0, LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<CHIPSET, LED_PIN_STRIP_2, COLOR_SEQUENCE>(leds, LEDS_PER_STRIP + 1, LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);

  set_max_power_indicator_LED(LED_BUILTIN); // turn on the builtin led when the power throttling kicks in
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILLI_AMPS);
  FastLED.setBrightness(g_Brightness);

  loadFieldsFromEEPROM(fields, fieldCount);
}

void loop()
{
  server.handleClient();
  // sensorValue = map(sensorValue, 0, 4095, 0, 255);
  if (g_Power == 0)
  {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
  }
  else
  {
    patterns[currentPatternIndex].pattern();
  }
  EVERY_N_MILLISECONDS(250)
  {
    g_OLED.clearBuffer();
    g_OLED.setCursor(0, g_lineHeight);
    g_OLED.print(getHostName());
    g_OLED.setCursor(0, g_lineHeight * 2);
    g_OLED.printf("FPS: %u", FastLED.getFPS());
    g_OLED.setCursor(0, g_lineHeight * 3);
    g_OLED.printf("MaxPower: %u mW", calculate_unscaled_power_mW(leds, NUM_LEDS));
    g_OLED.setCursor(0, g_lineHeight * 4);
    g_OLED.print("Pattern: ");
    g_OLED.print(patterns[currentPatternIndex].name);
    g_OLED.setCursor(0, g_lineHeight * 5);
    g_OLED.printf("Brightness: %s/%d", getBrightness(), calculate_max_brightness_for_power_mW(255, 5 * MILLI_AMPS));
    g_OLED.setCursor(0, g_lineHeight * 6);
    g_OLED.print("RGB: ");
    g_OLED.print(getSolidColor());
    g_OLED.sendBuffer();
  }
  EVERY_N_MILLISECONDS(1000)
  {
    writeFieldsToEEPROM(fields, fieldCount);
  }
  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}