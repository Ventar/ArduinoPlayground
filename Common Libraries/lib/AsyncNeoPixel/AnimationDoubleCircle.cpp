#include <AnimationDoubleCircle.h>

namespace leds {

void AnimationDoubleCircle::reset() {
  for (uint16_t i = 0; i < strip->numPixels(); i++) {
    strip->setPixelColor(i, data->colors[i]);
  }

  show();
}

void AnimationDoubleCircle::process() {
  if (counter > strip->numPixels()) {
    counter == 0;
  }

  for (uint16_t i = 0; i < strip->numPixels() / 2; i++) {
    strip->setPixelColor(i, strip->getPixelColor(i + 1));
  }

  counter++;

  show();
}
} // namespace leds
