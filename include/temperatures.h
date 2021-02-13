typedef struct TemperatureAndName
{
  ColorTemperature temperature;
  String name;
};
typedef TemperatureAndName TemperatureAndNameList[];

TemperatureAndNameList temperatures = {
    {UncorrectedTemperature, "UncorrectedTemperature"},
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
    {Candle, "Candle"},

};

uint8_t colorCount = ARRAY_SIZE(temperatures);
