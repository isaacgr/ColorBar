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
#define BRIGHTNESS_INC 19
#define BRIGHTNESS_DEC 18
#define RESET_BUTTON

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
uint8_t apmode = 0;

uint8_t currentPatternIndex = 0;
uint8_t currentTemperatureIndex = 0;
bool writeFields = false;
bool RESET = false;

// EEPROM addresses for state
const uint8_t SSID_INDEX = 1;
const uint8_t PASS_INDEX = 2;
const uint8_t WIFI_SET = 3;
const uint8_t MDNS_INDEX = 4;
const uint8_t MDNS_SET = 5;
const uint8_t AP_SET = 6;

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