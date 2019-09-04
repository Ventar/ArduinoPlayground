#include <AnimationStatic.h>

namespace leds {

void AnimationStatic::reset() {
  setAll(0, 0, 0);

  /*
    if (data->colors[0] == 0) {
      setColorListFromString("000044");
    }
    */

  for (uint16_t i = 0; i < strip->numPixels(); i++) {
    strip->setPixelColor(i, data->colors[i]);
  }

  show();

  lastColor = 0;
}

void AnimationStatic::process() {}

} // namespace leds