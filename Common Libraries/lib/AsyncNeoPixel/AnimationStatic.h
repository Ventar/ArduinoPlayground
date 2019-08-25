#ifndef AnimationStatic_h
#define AnimationStatic_h

#include <Animation.h>

namespace leds {

class AnimationStatic : public Animation {
public:
  /**
   * Constructor
   */
  AnimationStatic(Adafruit_NeoPixel *strip, uint16_t maxDelay, uint16_t speed)
      : Animation(strip, "Static", "Strip", "static", maxDelay, speed){};

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