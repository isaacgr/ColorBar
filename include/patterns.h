#include <Pacifica.h>
#include <Rainbow.h>
#include <ColorTemperature.h>
#include <Bounce.h>
#include <Fire.h>
#include <Comet.h>
#include <Marquee.h>

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
    {DrawBouncingBalls, "bouncingBalls"},
    {DrawFireEffect, "fire"},
    {DrawComet, "comet"},
    {DrawMarquee, "marquee"},
};

uint8_t patternCount = ARRAY_SIZE(patterns);
