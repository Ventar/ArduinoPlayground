#ifndef AnimationFire_h
#define AnimationFire_h

#include <Animation.h>

namespace leds {

/**
 * Fire animation.
 */
class AnimationFire : public Animation {
public:
  /**
   * Constructor
   */
  AnimationFire(Adafruit_NeoPixel *strip, uint16_t maxDelay, uint16_t speed)
      : Animation(strip, "Fire", "None", "fire", maxDelay, speed){};

  /**
   * Performs all update operations of the LEDs when the underlying delay is
   * passed.
   */
  void process() override;

private:
  void setPixelHeatColor(int Pixel, byte temperature);
};

} // namespace leds

#endif