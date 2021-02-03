#include <Pacifica.h>
#include <Rainbow.h>
#include <ColorTemperature.h>

void showRGB(const CRGB &rgb)
{
  Serial.printf("R: %d\n", rgb.r);
  Serial.printf("G: %d\n", rgb.g);
  Serial.printf("B: %d\n", rgb.b);
}

typedef void (*Pattern)();
typedef Pattern PatternList[];
typedef struct
{
  Pattern pattern;
  String name;
} PatternAndName;
typedef PatternAndName PatternAndNameList[];

PatternAndNameList patterns = {
    {DrawRainbow, "rainbow"},
    {pacifica_loop, "pacifica"},
    {DrawColorTemperature, "colorTemperature"},
};

uint8_t patternCount = ARRAY_SIZE(patterns);
