#ifndef AnimationCircle_h
#define AnimationCircle_h

#include <Animation.h>

namespace leds {

class AnimationCircle : public Animation {
public:
  /**
   * Constructor
   */
  AnimationCircle(Adafruit_NeoPixel *strip, uint16_t maxDelay, uint16_t speed)
      : Animation(strip, "Circle", "Strip", "circle", maxDelay, speed){};

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