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

#define FASTLED_INTERNAL
#define OLED_CLOCK 22
#define OLED_DATA 21
#define OLED_RESET 16

#define NUM_LEDS 20
#define LED_PIN 5
#define COLOR_SEQUENCE GRB
#define CHIPSET WS2812B

#define FRAMES_PER_SECOND 120

CRGB leds[NUM_LEDS] = {0};

U8G2_SSD1306_128X64_NONAME_F_HW_I2C g_OLED(U8G2_R0, OLED_RESET, OLED_CLOCK, OLED_DATA);
WebServer server(80);

uint8_t g_lineHeight = 0;
uint8_t g_Brightness = 255;
uint16_t g_PowerLimit = 900;
uint8_t g_Power = 1;
uint8_t currentPatternIndex = 0;
uint8_t currentTemperatureIndex = 0;

#include <secret.h>
#include <WiFiUtils.h>
#include <FileManager.h>
#include <patterns.h>

#include <Field.h>
#include <Fields.h>
#include <Web.h>

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  while (!Serial)
  {
  }
  setupWifi();
  setupMDNS();

  // SPIFFS.format(); // Prevents SPIFFS_ERR_NOT_A_FS
  SPIFFS.begin(); // Start the SPI Flash Files System

  server.begin();
  server.enableCORS();
  setupWeb();

  Serial.println("HTTP server started");
  g_OLED.clear();
  g_OLED.begin();
  g_OLED.setFont(u8g2_font_profont15_tf);
  g_lineHeight = g_OLED.getFontAscent() - g_OLED.getFontDescent(); // decent is a negative number, so subtract from the total

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_SEQUENCE>(leds, NUM_LEDS);

  set_max_power_indicator_LED(LED_BUILTIN); // turn on the builtin led when the power throttling kicks in
  FastLED.setMaxPowerInMilliWatts(g_PowerLimit);
  FastLED.setBrightness(g_Brightness);
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
  // EVERY_N_MILLISECONDS(250)
  // {
  //   g_OLED.clearBuffer();
  //   g_OLED.setCursor(0, g_lineHeight);
  //   g_OLED.printf("FPS: %u", FastLED.getFPS());
  //   g_OLED.setCursor(0, g_lineHeight * 2);
  //   g_OLED.printf("Power: %u mW", calculate_unscaled_power_mW(leds, NUM_LEDS));
  //   g_OLED.setCursor(0, g_lineHeight * 3);
  //   g_OLED.printf("Brite: %d", calculate_max_brightness_for_power_mW(255, g_PowerLimit));
  //   g_OLED.sendBuffer();
  // }
  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}