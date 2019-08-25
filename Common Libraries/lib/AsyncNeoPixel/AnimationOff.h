#ifndef AnimationOff_h
#define AnimationOff_h

#include <Animation.h>

namespace leds {

class AnimationOff : public Animation {
public:
  /**
   * Constructor
   */
  AnimationOff(Adafruit_NeoPixel *strip, uint16_t maxDelay, uint16_t speed)
      : Animation(strip, "Off", "None", "off", maxDelay, speed){};

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