#ifndef AnimationMeteorRain_h
#define AnimationMeteorRain_h

#include <Animation.h>

namespace leds {

/**
 * Meteor Rain animation.
 */
class AnimationMeteorRain : public Animation {
public:
  /**
   * Constructor
   */
  AnimationMeteorRain(Adafruit_NeoPixel *strip, uint16_t maxDelay,
                      uint16_t speed)
      : Animation(strip, "Meteor Rain", "Single", "meteor", maxDelay, speed){};

  /**
   * Resets the animation to the initial values.
   */
  void reset() override;

  /**
   * Performs all update operations of the LEDs when the underlying delay is
   * passed.
   */
  void process() override;

  void setMeteorSize(uint8_t size);
  void setMeteorDecay(uint8_t decay);
  void setMeteorRandomDecay(boolean random);

private:
  uint8_t meteorSize = 10;
  uint8_t meteorTrailDecay = 64;
  boolean meteorRandomDecay = true;
};

} // namespace leds

#endif