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
#include "defines.h"
#include "secrets.h"

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3003000)
#warning "Requires FastLED 3.3 or later; check github for latest code."
#endif

CRGB leds[NUM_LEDS] = {0};

U8G2_SSD1306_128X64_NONAME_F_HW_I2C g_OLED(U8G2_R0, OLED_RESET, OLED_CLOCK, OLED_DATA);
WebServer server(80);
uint8_t g_lineHeight = 0;
uint8_t g_Brightness = 128;
uint8_t g_Power = 1;
int BRIGHTNESS_INCREMENT = 16;
uint8_t apmode = 0;

uint8_t currentPatternIndex = 0;
uint8_t currentTemperatureIndex = 0;
bool writeFields = false;
bool RESET = false;

// modifiers for fire, water and pacifica effects
uint8_t g_ColorTemperature = 0;
uint8_t g_ColorPalette = 0;
uint8_t g_Sparking = 150;
uint8_t g_Cooling = 6;
uint8_t g_Sparks = 1;
uint8_t g_SparkHeight = 1;
bool breversed = false;
bool bmirrored = true;
bool g_Cycle = false;
uint8_t g_Speed = 20;

CRGB solidColor = CRGB::Red;

#include <eeprom_utils.h>
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
  static unsigned long rise_time;
  static unsigned long fall_time;

  unsigned long interrupt_time = millis();

  unsigned long toggle_ap_mode_time = 2500; // toggle ap mode if button held for 3s
  unsigned long factory_reset_time = 9500;  // clear eeprom if button held for 10s

  uint8_t pinState = digitalRead(POWER_BUTTON);
  if (pinState == 1)
  {
    rise_time = millis();
  }
  else
  {
    fall_time = millis();
    // If interrupts come faster than 200ms, assume it's a bounce and ignore
    if (interrupt_time - last_interrupt_time > 200)
    {
      unsigned long diff = fall_time - rise_time;
      if (diff < toggle_ap_mode_time)
      {
        // assume user wants power off
        g_Power = !g_Power;
        writeFields = true;
      }
      else if (diff >= toggle_ap_mode_time && diff < factory_reset_time)
      {
        apmode = !apmode;
        EEPROM.write(AP_SET, apmode);
        RESET = true;
      }
      else if (diff >= factory_reset_time)
      {
        for (int i = 0; i < 512; i++)
        {
          EEPROM.write(i, 255);
        }
        EEPROM.write(AP_SET, 0);
        RESET = true;
      }
    }
    last_interrupt_time = interrupt_time;
  }
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
  writeFields = true;
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
  writeFields = true;
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
  writeFields = true;
}

void setup()
{
  if (!EEPROM.begin(512))
  {
    Serial.println("Failed to initialize EEPROM!");
    return;
  }
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_PIN_STRIP_1, OUTPUT);
  pinMode(LED_PIN_STRIP_2, OUTPUT);
  pinMode(POWER_BUTTON, INPUT);
  pinMode(PATTERN_BUTTON, INPUT);
  pinMode(BRIGHTNESS_INC, INPUT);
  pinMode(BRIGHTNESS_DEC, INPUT);

  attachInterrupt(POWER_BUTTON, POWER_ISR, CHANGE);
  attachInterrupt(PATTERN_BUTTON, PATTERN_ISR, RISING);
  attachInterrupt(BRIGHTNESS_INC, BRIGHTNESS_INC_ISR, RISING);
  attachInterrupt(BRIGHTNESS_DEC, BRIGHTNESS_DEC_ISR, RISING);

  Serial.begin(115200);
  while (!Serial)
  {
  }
  apmode = EEPROM.read(AP_SET);

  setupMDNS();
  setupWifi();
  // SPIFFS.format(); // Prevents SPIFFS_ERR_NOT_A_FS
  SPIFFS.begin(); // Start the SPI Flash Files System

  server.begin();
  server.enableCORS();
  setupWeb();

  g_OLED.clear();
  g_OLED.begin();
  g_OLED.setFont(u8g2_font_profont11_tf);
  g_lineHeight = g_OLED.getFontAscent() - g_OLED.getFontDescent(); // decent is a negative number, so subtract from the total

  FastLED.addLeds<CHIPSET, LED_PIN_STRIP_1, COLOR_SEQUENCE>(leds, 0, LEDS_STRIP_1).setCorrection(TypicalLEDStrip);
  // FastLED.addLeds<CHIPSET, LED_PIN_STRIP_2, COLOR_SEQUENCE>(leds, LEDS_STRIP_1 + 1, LEDS_STRIP_2).setCorrection(TypicalLEDStrip);

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
  if (writeFields)
  {
    writeFieldsToEEPROM(fields, fieldCount);
    writeFields = false;
  }
  if (RESET)
  {
    RESET = !RESET;
    EEPROM.commit();
    ESP.restart();
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
    g_OLED.setCursor(0, g_lineHeight * 7);
    if (apmode == 0)
    {
      if (WiFi.status() == WL_CONNECTED)
      {
        g_OLED.println(WiFi.localIP());
      }
      else
      {
        g_OLED.println("No wifi");
      }
    }
    else
    {
      g_OLED.println("AP Mode");
    }
    g_OLED.sendBuffer();
  }
  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}