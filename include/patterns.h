#include <temperatures.h>
#include <pacifica.h>
#include <fire.h>

void showRGB(const CRGB &rgb)
{
  Serial.printf("R: %d\n", rgb.r);
  Serial.printf("G: %d\n", rgb.g);
  Serial.printf("B: %d\n", rgb.b);
}
/*
===============================
FastLEDs built in rainbow generator
===============================
*/
void DrawRainbow()
{
  fill_rainbow(leds, NUM_LEDS, g_Hue, g_Speed);
  FastLED.setTemperature(temperatures[currentTemperatureIndex].temperature);
  if (g_Cycle)
  {
    --g_Hue;
  }
  else
  {
    g_Hue = 0;
  }
}

/*
===============================
Marquee effect from Daves Garage
===============================
*/
void DrawMarquee()
{
  static byte j = HUE_BLUE;
  j += 4;
  byte k = j;
  CRGBPalette16 palette = palettes[currentPaletteIndex];
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(palette, k += 8);
  }

  static int scroll = 0;
  scroll++;

  for (int i = scroll % 5; i < NUM_LEDS - 1; i += 5)
  {
    leds[i] = CRGB::Black;
  }
  FastLED.delay(50);
}

/*
===============================
Solid color
===============================
*/
void showSolidColor()
{
  fill_solid(leds, NUM_LEDS, solidColor);
}

/*
===============================
Fire effect from Daves garage
===============================
*/

void DrawFireEffect()
{
  FastLED.clear();
  DrawFire(HeatColors_p);
};

/*
===============================
Water effect
===============================
*/

void DrawWaterEffect()
{
  FastLED.clear();
  DrawFire(IceColors_p);
};

/*
===============================
Colored stripes pulsing at
a defined bpm
===============================
*/
void bpm()
{
  uint8_t beat = beatsin8(g_Speed, 64, 255);
  CRGBPalette16 palette = palettes[currentPaletteIndex];
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(palette, g_Hue + (i * 2), beat - g_Hue + (i * 10));
  }
}
/*
===============================
Colored dot sweeping back and
forth with fading trails
===============================
*/
void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(g_Speed, 0, NUM_LEDS - 1);
  static int prevpos = 0;
  CRGB color = ColorFromPalette(palettes[currentPaletteIndex], g_Hue, 255);
  if (pos < prevpos)
  {
    fill_solid(leds + pos, (prevpos - pos) + 1, color);
  }
  else
  {
    fill_solid(leds + prevpos, (pos - prevpos) + 1, color);
  }
  prevpos = pos;
}

/*
===============================
Comet effect
===============================
*/
void DrawComet()
{
  const byte fadeAmt = 128;
  const int cometSize = 5;
  const int deltaHue = 4;

  static byte hue = HUE_RED;
  static int iDirection = 1;
  static int iPos = 0;

  hue += deltaHue;
  iPos += iDirection;

  if (iPos == (NUM_LEDS - cometSize) || iPos == 0)
  {
    iDirection *= -1;
  }

  for (int i = 0; i < cometSize; i++)
  {
    leds[iPos + i].setHue(hue);
  }

  for (int j = 0; j < NUM_LEDS; j++)
  {
    if (random(10) > 5)
    {
      leds[j] = leds[j].fadeToBlackBy(fadeAmt);
    }
  }
  FastLED.delay(g_Speed);
}

typedef void (*Pattern)();
typedef Pattern PatternList[];
typedef struct
{
  Pattern pattern;
  String name;
  String modifiers;
} PatternAndName;
typedef PatternAndName PatternAndNameList[];

PatternAndNameList patterns = {
    {DrawRainbow, "rainbow", "colorTemperature,cycling,speed"},
    {pacifica_loop, "pacifica", ""},
    {DrawFireEffect, "fire", "sparking,cooling,sparks,sparkHeight,reversed,mirrored"},
    {DrawWaterEffect, "water", "sparking,cooling,sparks,sparkHeight,reversed,mirrored"},
    {DrawMarquee, "marquee", "colorTemperature,palette"},
    {showSolidColor, "solidColor", ""},
    {bpm, "bpm", "palette,speed"},
    {sinelon, "sinelon", "palette,speed"},
    {DrawComet, "comet", "speed"},
};

uint8_t patternCount = ARRAY_SIZE(patterns);
