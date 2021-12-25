// transition all LEDs at once through the color wheel

#define FASTLED_ALLOW_INTERRUPTS 0
FASTLED_USING_NAMESPACE

uint8_t initialHue = 0;
const uint8_t deltaHue = 16;
const uint8_t hueDensity = 1;

void DrawRainbow()
{
  fill_rainbow(leds, NUM_LEDS, initialHue += hueDensity, deltaHue);
}

void DrawFillRainbow()
{
  // fill_solid(leds, NUM_LEDS, initialColor);
  initialHue++;
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV(initialHue, 255, 255);
  }
  FastLED.delay(100);
}