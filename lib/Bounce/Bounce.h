#include <sys/time.h> // For time-of-day
using namespace std;
#include <vector> // Use the C++ resizable array

// BouncingBallEffect
//
// Draws a set of N bouncing balls using a simple little kinematics formula.  Clears the section first.

static const CRGB ballColors[] =
    {
        CRGB::Green,
        CRGB::Red,
        CRGB::Blue,
        CRGB::Orange,
        CRGB::Indigo,
        CRGB::Cyan,
};

class BouncingBallEffect
{
private:
  double InitialBallSpeed(double height) const
  {
    return sqrt(-2 * Gravity * height);
  }

  size_t _cLength;
  size_t _cBalls;
  byte _Fade;
  bool _bMirrored;
  double _SpeedKnob;

  const double Gravity = -9.81;                                     // Because PHYSICS!
  const double StartHeight = 1;                                     // Drop balls from max height to start
  const double ImpactVelocityStart = InitialBallSpeed(StartHeight); // Speed for  a
  const double SpeedKnob = 4;                                       // High number will slow effect down

  vector<double> ClockTimeAtLastBounce, Height, BallSpeed, Dampening; // When the ball last bounced
  vector<CRGB> Colors;

  // Time - Return current time in floating form for easier calcs than ms

  double Time() const
  {
    timeval tv = {0};
    gettimeofday(&tv, nullptr);
    return (double)(tv.tv_usec / (double)1000000 + (double)tv.tv_sec);
  }

public:
  // BouncingBallEffect
  //
  // Caller specs strip length, number of balls, persistence level (255 is least), and whether
  // the balls should be drawn mirrored from each side.

  BouncingBallEffect(size_t cLength, size_t ballCount = 3, byte fade = 0, bool bMirrored = false, double SpeedKnob = 4.0)
      : _cLength(cLength - 1), // Reserve one LED for floating point fraction draw
        _cBalls(ballCount),
        _Fade(fade),
        _bMirrored(bMirrored),
        _SpeedKnob(SpeedKnob),
        ClockTimeAtLastBounce(ballCount),
        Height(ballCount),
        BallSpeed(ballCount),
        Dampening(ballCount),
        Colors(ballCount)
  {

    for (size_t i = 0; i < ballCount; i++)
    {
      Height[i] = StartHeight;                    // Current ball height
      ClockTimeAtLastBounce[i] = Time();          // When the last time it hit ground was
      Dampening[i] = 1.0 - i / pow(_cBalls, 2);   // Each ball bounces differently
      BallSpeed[i] = InitialBallSpeed(Height[i]); // Don't dampen initial launch to they go together
      Colors[i] = ballColors[i % ARRAY_SIZE(ballColors)];
    }
  }

  // Draw
  //
  // Draw each of the balls.  When any ball gets too little energy it would just sit at the base so it is re-kicked with new energy.#pragma endregion

  virtual void Draw()
  {
    if (_Fade)
    {
      for (size_t i = 0; i < _cLength; i++)
      {
        leds[i].fadeToBlackBy(_Fade);
      }
    }
    else
    {
      FastLED.clear();
    }

    // Draw each of the three balls
    for (size_t i = 0; i < _cBalls; i++)
    {
      double TimeSinceLastBounce = (Time() - ClockTimeAtLastBounce[i]) / _SpeedKnob;
      Height[i] = 0.5 * Gravity * pow(TimeSinceLastBounce, 2.0) + BallSpeed[i] * TimeSinceLastBounce;

      if (Height[i] < 0)
      {
        Height[i] = 0;
        BallSpeed[i] = Dampening[i] * BallSpeed[i];
        ClockTimeAtLastBounce[i] = Time();

        if (BallSpeed[i] < 1.0)
          BallSpeed[i] = InitialBallSpeed(StartHeight) * Dampening[i];
      }

      size_t position = (Height[i] * (_cLength - 1.0) / StartHeight);

      leds[position] += Colors[i];
      leds[position + 1] += Colors[i];

      if (_bMirrored)
      {
        leds[_cLength - 1 - position] += Colors[i];
        leds[_cLength - position] += Colors[i];
      }
    }
  }
};

BouncingBallEffect balls(NUM_LEDS, 3, 0, false, 8.0);

void DrawBouncingBalls()
{
  balls.Draw();
};