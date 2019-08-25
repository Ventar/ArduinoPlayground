#ifndef AnimationSparkle_h
#define AnimationSparkle_h

#include <Animation.h>

namespace leds {

/**
 * Sparkle animation.
 */
class AnimationSparkle : public Animation {
public:
  /**
   * Constructor.
   */
  AnimationSparkle(Adafruit_NeoPixel *strip, uint16_t maxDelay, uint16_t speed)
      : Animation(strip, "Sparkle", "Strip", "sparkle", maxDelay, speed) {
    memory = new uint8_t[45]{0};
  };

  /**
   * Resets the animation to the initial values.
   */
  void reset() override;

  /**
   * Performs all update operations of the LEDs when the underlying delay is
   * passed.
   */
  void process() override;

private:
  uint8_t *memory;
};

} // namespace leds
#endif