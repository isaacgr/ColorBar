#include <FastLED.h>
#include "leds.h"

CRGB parseColor(String value)
{
  uint8_t ri = value.indexOf(",");
  uint8_t gi = value.indexOf(",", ri + 1);

  String rs = value.substring(0, ri);
  String gs = value.substring(ri + 1, gi);
  String bs = value.substring(gi + 1);

  uint8_t r = rs.toInt();
  uint8_t g = gs.toInt();
  uint8_t b = bs.toInt();

  return CRGB(r, g, b);
}