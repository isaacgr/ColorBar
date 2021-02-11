void DrawMarquee()
{
  static byte j = HUE_BLUE;
  j += 4;
  byte k = j;

  CRGB c;
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = c.setHue(k += 8);
  }

  static int scroll = 0;
  scroll++;

  for (int i = scroll % 5; i < NUM_LEDS - 1; i += 5)
  {
    leds[i] = CRGB::Black;
  }
  delay(50);
}