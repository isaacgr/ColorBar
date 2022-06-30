#include <ledgfx.h>

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