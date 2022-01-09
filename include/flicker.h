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
  CRGB targetWhite(93, 0, 255);
  CRGB targetBlack(0, 0, 0);

  CRGB bgColor(0, 15, 2); // pine green ?

  // fade all existing pixels toward bgColor by "5" (out of 255)
  fadeTowardColor(leds, NUM_LEDS, targetBlack, 5);

  // periodically set random pixel to a random color, to show the fading
  uint16_t pos = random16(NUM_LEDS);
  CRGB color = targetWhite;
  leds[pos] = color;
}