#include <Arduino.h>
#include <EEPROM.h>
#include <U8g2lib.h>
#include <FastLED.h>
#include <FS.h>
#include <SPIFFS.h>
#include "defines.h"
#include "eeprom_utils.h"
#include "routes.h"
#include "wifi_utils.h"
#include "leds.h"
#include "field_utils.h"
#include "pattern.h"

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3003000)
#warning "Requires FastLED 3.3 or later; check github for latest code."
#endif

WebServer server(80);

U8G2_SSD1306_128X64_NONAME_F_HW_I2C g_OLED(U8G2_R0, OLED_RESET, OLED_CLOCK, OLED_DATA);

uint8_t apmode = 0;
uint8_t rgbmode = 0;
bool writeFields = false;
bool RESET = false;
uint8_t g_lineHeight = 0;
uint8_t oled_i2c_addr = 0x78; // address for oled display

uint8_t g_Brightness = 128;
uint8_t g_Power = 1;
uint8_t currentPatternIndex = 0;

CRGB leds[NUM_LEDS] = {0};
CRGB solidColor = CRGB::Red;

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

PatternAndNameList patterns = {
    {showSolidColor, "solidColor", ""},
    {pacifica_loop, "pacifica", ""},
    {DrawFireEffect, "fire", ""},
    {DrawWaterEffect, "water", ""},
    {DrawRainbowEffect, "rainbow", ""},
    {DrawFillRainbowEffect, "rainbow2", ""},
    {DrawFlicker1, "flicker1", ""},
    {DrawFlicker2, "flicker2", ""},
};

uint8_t patternCount = ARRAY_SIZE(patterns);

FieldList fields = {
    {"power", "Power", NumberFieldType, false, 0, 1, getPower, NULL, setPower},
    {"brightness", "Brightness", NumberFieldType, false, 1, 255, getBrightness, NULL, setBrightness},
    {"pattern", "Pattern", SelectFieldType, false, 0, patternCount, getPattern, getPatterns, setPattern, setPatternByValue, getPatternIndex},
    {"solidColor", "SolidColor", ColorFieldType, false, 0, 255, getSolidColor, NULL, setSolidColor},
};

uint8_t fieldCount = ARRAY_SIZE(fields);

void IRAM_ATTR POWER_ISR()
{
  static unsigned long last_interrupt_time = 0;
  static unsigned long rise_time;
  static unsigned long fall_time;

  unsigned long interrupt_time = millis();

  unsigned long toggle_ap_mode_time = 5000; // toggle the access point mode if button held for 5s
  unsigned long rgb_mode = 3000;            // toggle rgb edit mode if button held for 3s
  unsigned long factory_reset_time = 10000; // clear eeprom if button held for 10s

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
      else if (diff >= rgb_mode && diff < toggle_ap_mode_time)
      {
        apmode = !apmode;
        EEPROM.write(AP_SET, apmode);
        RESET = true;
      }
      else if (diff >= toggle_ap_mode_time && diff < factory_reset_time)
      {
        rgbmode = !rgbmode;
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
    if (rgbmode)
    {
      if (solidColor.r >= 255)
      {
        solidColor = CRGB(0, solidColor.g, solidColor.b);
      }
      else
      {
        solidColor = CRGB(solidColor.r += 16, solidColor.g, solidColor.b);
      }
    }
    else
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
    if (rgbmode)
    {
      if (solidColor.g >= 255)
      {
        solidColor = CRGB(solidColor.r, 0, solidColor.b);
      }
      else
      {
        solidColor = CRGB(solidColor.r, solidColor.g += 16, solidColor.b);
      }
    }
    else
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
    if (rgbmode)
    {
      if (solidColor.b >= 255)
      {
        solidColor = CRGB(solidColor.r, solidColor.g, 0);
      }
      else
      {
        solidColor = CRGB(solidColor.r, solidColor.g, solidColor.b += 16);
      }
    }
    else
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
  setupWifi(apmode);
  // Try to mount SPIFFS without formatting on failure
  if (!SPIFFS.begin(false))
  {
    // If SPIFFS does not work, we wait for serial connection...
    while (!Serial)
      ;
    delay(1000);

    // Ask to format SPIFFS using serial interface
    Serial.print("Mounting SPIFFS failed. Try formatting? (y/n): ");
    while (!Serial.available())
      ;
    Serial.println();

    // If the user did not accept to try formatting SPIFFS or formatting failed:
    if (Serial.read() != 'y' || !SPIFFS.begin(true))
    {
      Serial.println("SPIFFS not available. Stop.");
      while (true)
        ;
    }
    else
    {
      SPIFFS.format();
    }
    Serial.println("SPIFFS has been formated.");
  }

  setupWeb();

  g_OLED.setI2CAddress(oled_i2c_addr);
  g_OLED.clear();
  g_OLED.begin();
  g_OLED.setFont(u8g2_font_profont11_tf);
  g_lineHeight = g_OLED.getFontAscent() - g_OLED.getFontDescent(); // decent is a negative number, so subtract from the total

  FastLED.addLeds<CHIPSET, LED_PIN_STRIP_1, COLOR_SEQUENCE>(leds, 0, LEDS_STRIP_1).setCorrection(TypicalLEDStrip);
  // FastLED.addLeds<CHIPSET, LED_PIN_STRIP_2, COLOR_SEQUENCE>(leds, LEDS_STRIP_1 + 1, LEDS_STRIP_2).setCorrection(TypicalLEDStrip);

  set_max_power_indicator_LED(LED_BUILTIN); // turn on the builtin led when the power throttling kicks in
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILLI_AMPS);
  FastLED.setBrightness(g_Brightness);

  loadFieldsFromEEPROM();
}

void loop()
{
  server.handleClient();
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
    writeFieldsToEEPROM();
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
    if (!rgbmode)
    {
      g_OLED.setCursor(0, g_lineHeight);
      g_OLED.print(getHostname());
      g_OLED.printf(" FPS: %u", FastLED.getFPS());
      // g_OLED.setCursor(0, g_lineHeight * 2);
      // g_OLED.printf("FPS: %u  mem: %d", FastLED.getFPS(), ESP.getFreeHeap());
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
    }
    else
    {
      g_OLED.setCursor(0, g_lineHeight);
      g_OLED.print("RGB: ");
      g_OLED.print(getSolidColor());
    }
    g_OLED.sendBuffer();
  }
  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}