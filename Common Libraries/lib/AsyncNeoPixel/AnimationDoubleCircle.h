#ifndef AnimationDoubleCircle_h
#define AnimationDoubleCircle_h

#include <Animation.h>

namespace leds {

class AnimationDoubleCircle : public Animation {
public:
  /**
   * Constructor
   */
  AnimationDoubleCircle(Adafruit_NeoPixel *strip, uint16_t maxDelay,
                        uint16_t speed)
      : Animation(strip, "Double Circle", "Strip", "double circle", maxDelay,
                  speed){};

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