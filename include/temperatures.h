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
