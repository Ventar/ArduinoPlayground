#include <AnimationCircle.h>

namespace leds {

void AnimationCircle::reset() {
  for (uint16_t i = 0; i < strip->numPixels(); i++) {
    strip->setPixelColor(i, data->colors[i]);
  }

  show();
}

void AnimationCircle::process() {
  uint32_t start = strip->getPixelColor(0);

  for (uint16_t i = 0; i < strip->numPixels() - 1; i++) {
    strip->setPixelColor(i, strip->getPixelColor(i + 1));
  }

  strip->setPixelColor(strip->numPixels() - 1, start);
  show();
}

} // namespace leds