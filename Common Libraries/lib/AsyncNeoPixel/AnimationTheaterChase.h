#ifndef AnimationTheaterChase_h
#define AnimationTheaterChase_h

#include <Animation.h>

namespace leds {

/**
 * Theater Chase animation.
 */
class AnimationTheaterChase : public Animation {
public:
  /**
   * Constructor.
   */
  AnimationTheaterChase(Adafruit_NeoPixel *strip, uint16_t maxDelay,
                        uint16_t speed)
      : Animation(strip, "Theater Chase", "Strip", "theater", maxDelay,
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