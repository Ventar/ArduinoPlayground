#ifndef AnimationCloud_h
#define AnimationCloud_h

#include <Animation.h>

namespace leds {

class AnimationCloud : public Animation {
public:
  /**
   * Constructor
   */
  AnimationCloud(Adafruit_NeoPixel *strip, uint16_t maxDelay, uint16_t speed)
      : Animation(strip, "Cloud", "Strip", "cloud", maxDelay, speed){};

  /**
   * Resets the animation to the initial values.
   */
  void reset() override;

  /**
   * Performs all update operations of the LEDs when the underlying delay is
   * passed.
   */
  void process() override;
};

} // namespace leds

#endif