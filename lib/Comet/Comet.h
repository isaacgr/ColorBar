void DrawComet()
{
  const byte fadeAmt = 64;
  const int cometSize = 5;
  const int deltaHue = 4;
  const double cometSpeed = 0.5;

  static byte hue = HUE_RED;
  static int iDirection = 1;
  static double iPos = 0.0;

  hue += deltaHue;
  iPos += iDirection * cometSpeed;

  if (iPos == (NUM_LEDS - cometSize) || iPos == 0)
  {
    iDirection *= -1;
  }

  for (int i = 0; i < cometSize; i++)
  {
    leds[(int)iPos + i].setHue(hue);
  }

  for (int j = 0; j < NUM_LEDS; j++)
  {
    if (random(2) == 1)
    {
      leds[j] = leds[j].fadeToBlackBy(fadeAmt);
    }
  }
  delay(20);
}