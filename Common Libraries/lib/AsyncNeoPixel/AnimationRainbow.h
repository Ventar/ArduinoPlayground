#ifndef AnimationRainbow_h
#define AnimationRainbow_h

#include <Animation.h>

namespace leds {

/**
 * Rainbow animation.
 */
class AnimationRainbow : public Animation {
public:
  /**
   * Constructor.
   */
  AnimationRainbow(Adafruit_NeoPixel *strip, uint16_t maxDelay, uint16_t speed)
      : Animation(strip, "Rainbow", "None", "rainbow", maxDelay, speed){};

  /**
   * Performs all update operations of the LEDs when the underlying delay is
   * passed.
   */
  void process() override;

private:
  uint8_t *wheel(uint8_t WheelPos);
};

} // namespace leds

#endif