#include <FastLED.h>
#include "pattern.h"
#include "leds.h"

uint8_t initialHue = 0;
const uint8_t deltaHue = 16;
const uint8_t hueDensity = 1;

PatternAndNameList patterns = {
    {showSolidColor, "solidColor", ""},
    {pacifica_loop, "pacifica", ""},
    {DrawFireEffect, "fire", ""},
    {DrawRainbowEffect, "rainbow", ""},
    {DrawFillRainbowEffect, "rainbow2", ""},
    {DrawFlicker1, "flicker1", ""},
    {DrawFlicker2, "flicker2", ""},
};

uint8_t patternCount = ARRAY_SIZE(patterns);

void DrawRainbow()
{
  fill_rainbow(leds, NUM_LEDS, initialHue += hueDensity, deltaHue);
}

void DrawFillRainbow()
{
  // transition all LEDs at once through the color wheel
  initialHue++;
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV(initialHue, 255, 255);
  }
  FastLED.delay(100);
}

//
//  "Pacifica"
//  Gentle, blue-green ocean waves.
//  December 2019, Mark Kriegsman and Mary Corey March.
//  For Dan.
//

#define FASTLED_ALLOW_INTERRUPTS 0
FASTLED_USING_NAMESPACE

//////////////////////////////////////////////////////////////////////////
//
// The code for this animation is more complicated than other examples, and
// while it is "ready to run", and documented in general, it is probably not
// the best starting point for learning.  Nevertheless, it does illustrate some
// useful techniques.
//
//////////////////////////////////////////////////////////////////////////
//
// In this animation, there are four "layers" of waves of light.
//
// Each layer moves independently, and each is scaled separately.
//
// All four wave layers are added together on top of each other, and then
// another filter is applied that adds "whitecaps" of brightness where the
// waves line up with each other more.  Finally, another pass is taken
// over the led array to 'deepen' (dim) the blues and greens.
//
// The speed and scale and motion each layer varies slowly within independent
// hand-chosen ranges, which is why the code has a lot of low-speed 'beatsin8' functions
// with a lot of oddly specific numeric ranges.
//
// These three custom blue-green color palettes were inspired by the colors found in
// the waters off the southern coast of California, https://goo.gl/maps/QQgd97jjHesHZVxQ7
//
CRGBPalette16 pacifica_palette_1 =
    {0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117,
     0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x14554B, 0x28AA50};
CRGBPalette16 pacifica_palette_2 =
    {0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117,
     0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x0C5F52, 0x19BE5F};
CRGBPalette16 pacifica_palette_3 =
    {0x000208, 0x00030E, 0x000514, 0x00061A, 0x000820, 0x000927, 0x000B2D, 0x000C33,
     0x000E39, 0x001040, 0x001450, 0x001860, 0x001C70, 0x002080, 0x1040BF, 0x2060FF};

// Add one layer of waves into the led array
void pacifica_one_layer(CRGBPalette16 &p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff)
{
  uint16_t ci = cistart;
  uint16_t waveangle = ioff;
  uint16_t wavescale_half = (wavescale / 2) + 20;
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    waveangle += 250;
    uint16_t s16 = sin16(waveangle) + 32768;
    uint16_t cs = scale16(s16, wavescale_half) + wavescale_half;
    ci += cs;
    uint16_t sindex16 = sin16(ci) + 32768;
    uint8_t sindex8 = scale16(sindex16, 240);
    CRGB c = ColorFromPalette(p, sindex8, bri, LINEARBLEND);
    leds[i] += c;
  }
}

// Add extra 'white' to areas where the four layers of light have lined up brightly
void pacifica_add_whitecaps()
{
  uint8_t basethreshold = beatsin8(9, 55, 65);
  uint8_t wave = beat8(7);

  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    uint8_t threshold = scale8(sin8(wave), 20) + basethreshold;
    wave += 7;
    uint8_t l = leds[i].getAverageLight();
    if (l > threshold)
    {
      uint8_t overage = l - threshold;
      uint8_t overage2 = qadd8(overage, overage);
      leds[i] += CRGB(overage, overage2, qadd8(overage2, overage2));
    }
  }
}

// Deepen the blues and greens
void pacifica_deepen_colors()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i].blue = scale8(leds[i].blue, 145);
    leds[i].green = scale8(leds[i].green, 200);
    leds[i] |= CRGB(2, 5, 7);
  }
}

void pacifica_loop()
{
  FastLED.clear();
  // Increment the four "color index start" counters, one for each wave layer.
  // Each is incremented at a different speed, and the speeds vary over time.
  static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
  static uint32_t sLastms = 0;
  uint32_t ms = GET_MILLIS();
  uint32_t deltams = ms - sLastms;
  sLastms = ms;
  uint16_t speedfactor1 = beatsin16(3, 179, 269);
  uint16_t speedfactor2 = beatsin16(4, 179, 269);
  uint32_t deltams1 = (deltams * speedfactor1) / 256;
  uint32_t deltams2 = (deltams * speedfactor2) / 256;
  uint32_t deltams21 = (deltams1 + deltams2) / 2;
  sCIStart1 += (deltams1 * beatsin88(1011, 10, 13));
  sCIStart2 -= (deltams21 * beatsin88(777, 8, 11));
  sCIStart3 -= (deltams1 * beatsin88(501, 5, 7));
  sCIStart4 -= (deltams2 * beatsin88(257, 4, 6));

  // Clear out the LED array to a dim background blue-green
  fill_solid(leds, NUM_LEDS, CRGB(2, 6, 10));

  // Render each of four layers, with different scales and speeds, that vary over time
  pacifica_one_layer(pacifica_palette_1, sCIStart1, beatsin16(3, 11 * 256, 14 * 256), beatsin8(10, 70, 130), 0 - beat16(301));
  pacifica_one_layer(pacifica_palette_2, sCIStart2, beatsin16(4, 6 * 256, 9 * 256), beatsin8(17, 40, 80), beat16(401));
  pacifica_one_layer(pacifica_palette_3, sCIStart3, 6 * 256, beatsin8(9, 10, 38), 0 - beat16(503));
  pacifica_one_layer(pacifica_palette_3, sCIStart4, 5 * 256, beatsin8(8, 10, 28), beat16(601));

  // Add brighter 'whitecaps' where the waves lines up more
  pacifica_add_whitecaps();

  // Deepen the blues and greens a bit
  pacifica_deepen_colors();
}

#include <sys/time.h> // For time-of-day

inline float RandomFloat()
{
  float r = random(1000000L) / 1000000.0f;
  return r;
}

inline double UnixTime()
{
  timeval tv = {0};
  gettimeofday(&tv, nullptr);
  return (double)(tv.tv_usec / 1000000.0 + (double)tv.tv_sec);
}

// FractionalColor
//
// Returns a fraction of a color; abstracts the fadeToBlack out to this function in case we
// want to improve the color math or do color correction all in one location at a later date.

CRGB ColorFraction(CRGB colorIn, float fraction)
{
  fraction = min(1.0f, fraction);
  return CRGB(colorIn).fadeToBlackBy(255 * (1.0f - fraction));
}

void DrawPixels(float fPos, float count, CRGB color)
{
  // Calculate how much the first pixel will hold
  float availFirstPixel = 1.0f - (fPos - (long)(fPos));
  float amtFirstPixel = min(availFirstPixel, count);
  float remaining = min(count, NUM_LEDS - fPos);
  int iPos = fPos;

  // Blend (add) in the color of the first partial pixel

  if (remaining > 0.0f)
  {
    FastLED.leds()[iPos++] += ColorFraction(color, amtFirstPixel);
    remaining -= amtFirstPixel;
  }

  // Now draw any full pixels in the middle

  while (remaining > 1.0f)
  {
    FastLED.leds()[iPos++] += color;
    remaining--;
  }

  // Draw tail pixel, up to a single full pixel

  if (remaining > 0.0f)
  {
    FastLED.leds()[iPos] += ColorFraction(color, remaining);
  }
}

// Helper function that blends one uint8_t toward another by a given amount
void nblendU8TowardU8(uint8_t &cur, const uint8_t target, uint8_t amount)
{
  if (cur == target)
    return;

  if (cur < target)
  {
    uint8_t delta = target - cur;
    delta = scale8_video(delta, amount);
    cur += delta;
  }
  else
  {
    uint8_t delta = cur - target;
    delta = scale8_video(delta, amount);
    cur -= delta;
  }
}

// Blend one CRGB color toward another CRGB color by a given amount.
// Blending is linear, and done in the RGB color space.
// This function modifies 'cur' in place.
CRGB fadeTowardColor(CRGB &cur, const CRGB &target, uint8_t amount)
{
  nblendU8TowardU8(cur.red, target.red, amount);
  nblendU8TowardU8(cur.green, target.green, amount);
  nblendU8TowardU8(cur.blue, target.blue, amount);
  return cur;
}

// Fade an entire array of CRGBs toward a given background color by a given amount
// This function modifies the pixel array in place.
void fadeTowardColor(CRGB *L, uint16_t N, const CRGB &bgColor, uint8_t fadeAmount)
{
  for (uint16_t i = 0; i < N; i++)
  {
    fadeTowardColor(L[i], bgColor, fadeAmount);
  }
}

void DrawFlicker1()
{
  CRGB targetRed(255, 0, 40);
  CRGB targetBlue(0, 255, 255);

  CRGB bgColor(0, 15, 2); // pine green ?

  // fade all existing pixels toward bgColor by "5" (out of 255)
  fadeTowardColor(leds, NUM_LEDS, targetBlue, 5);

  // periodically set random pixel to a random color, to show the fading
  uint16_t pos = random16(NUM_LEDS);
  CRGB color = targetRed;
  leds[pos] = color;
}

void DrawFlicker2()
{
#define NUM_COLORS 4

  CRGB colors[NUM_COLORS] = {
      CRGB(179, 0, 12),
      CRGB(220, 61, 42),
      // CRGB(13, 239, 66),
      CRGB(0, 179, 44),
      CRGB(13, 89, 1)};

  fadeTowardColor(leds, NUM_LEDS, colors[random8(NUM_COLORS)], 5);

  // periodically set random pixel to a random color, to show the fading
  uint16_t pos = random16(NUM_LEDS);
  leds[pos] = colors[random8(NUM_COLORS)];
}

// when diffusing the fire upwards, these control how much to blend in from teh cells below
// you can tune these to control how quickly and smoothly the fire spreads

static const byte BlendSelf = 2;
static const byte BlendNeighbor1 = 3;
static const byte BlendNeighbor2 = 2;
static const byte BlendNeighbor3 = 1;
static const byte BlendTotal = (BlendSelf + BlendNeighbor1 + BlendNeighbor2 + BlendNeighbor3);
static byte heat[NUM_LEDS] = {0};

void DrawFire(CRGBPalette16 palette)
{
  int size = NUM_LEDS;
  if (bmirrored)
    size = NUM_LEDS / 2;
  // first cool each cell by a little bit
  for (int i = 0; i < size; i++)
    heat[i] = max(0L, heat[i] - random(0, ((g_Cooling * 10) / size) + 2));

  // Next drift heat up and diffuse it a little bit
  for (int i = 0; i < size; i++)
    heat[i] = (heat[i] * BlendSelf +
               heat[(i + 1) % size] * BlendNeighbor1 +
               heat[(i + 2) % size] * BlendNeighbor2 +
               heat[(i + 3) % size] * BlendNeighbor3) /
              BlendTotal;
  // Randomly ignite new sparks down in the flame kernel
  for (int i = 0; i < g_Sparks; i++)
  {
    if (random(255) < g_Sparking)
    {
      int y = size - 1 - random(g_SparkHeight);
      heat[y] = heat[y] + random(160, 180); // This randomly rolls over sometimes of course, and that's essential to the effect
    }
  }

  // Finally convert heat to a color
  for (int i = 0; i < size; i++)
  {
    // CRGB color = HeatColor(heat[i]);
    CRGB color = ColorFromPalette(palette, heat[i]);
    int j = breversed ? (size - 1 - i) : i;
    DrawPixels(j, 1, color);
    if (bmirrored)
    {
      int j2 = !breversed ? (2 * size - 1 - i) : size + i;
      DrawPixels(j2, 1, color);
    }
  }
  FastLED.delay(80);
}
