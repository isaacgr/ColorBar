typedef struct TemperatureAndName
{
  ColorTemperature temperature;
  String name;
};
typedef TemperatureAndName TemperatureAndNameList[];

TemperatureAndNameList temperatures = {
    {Tungsten100W, "tungsten100W"},
    {OvercastSky, "overcastSky"},
};

uint8_t colorCount = ARRAY_SIZE(temperatures);

void DrawColorTemperature()
{
  // draw a generic, no-name rainbow
  static uint8_t starthue = 0;
  fill_rainbow(leds + 5, NUM_LEDS - 5, --starthue, 20);

  FastLED.setTemperature(temperatures[currentTemperatureIndex].temperature);
}