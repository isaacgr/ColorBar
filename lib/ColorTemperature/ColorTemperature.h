typedef struct TemperatureAndName
{
  ColorTemperature temperature;
  String name;
};
typedef TemperatureAndName TemperatureAndNameList[];

TemperatureAndNameList temperatures = {
    {Tungsten100W, "Tungsten100W"},
    {OvercastSky, "OvercastSky"},
    {Tungsten40W, "Tungsten40W"},
    {Halogen, "Halogen"},
    {CarbonArc, "CarbonArc"},
    {HighNoonSun, "HighNoonSun"},
    {ClearBlueSky, "ClearBlueSky"},
    {WarmFluorescent, "WarmFluorescent"},
    {StandardFluorescent, "StandardFluorescent"},
    {CoolWhiteFluorescent, "CoolWhiteFluorescent"},
    {FullSpectrumFluorescent, "FullSpectrumFluorescent"},
    {GrowLightFluorescent, "GrowLightFluorescent"},
    {BlackLightFluorescent, "BlackLightFluorescent"},
    {MercuryVapor, "MercuryVapor"},
    {SodiumVapor, "SodiumVapor"},
    {MetalHalide, "MetalHalide"},
    {HighPressureSodium, "HighPressureSodium"},
    {UncorrectedTemperature, "UncorrectedTemperature"},
    {Candle, "Candle"},

};

uint8_t colorCount = ARRAY_SIZE(temperatures);

void DrawColorTemperature()
{
  // draw a generic, no-name rainbow
  static uint8_t starthue = 0;
  fill_rainbow(leds + 5, NUM_LEDS - 5, --starthue, 20);

  FastLED.setTemperature(temperatures[currentTemperatureIndex].temperature);
}
