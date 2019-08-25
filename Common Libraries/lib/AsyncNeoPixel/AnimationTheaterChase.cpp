#include <AnimationTheaterChase.h>
#include <Arduino.h>

namespace leds {

void AnimationTheaterChase::reset() {
  setAll(0, 0, 0);

  if (data->colors[0] == 0) {
    setColorListFromString("000044");
  }

  counter = 0;
}

void AnimationTheaterChase::process() {
  for (int i = 0; i < strip->numPixels(); i++) {
    setPixel(i, 0);
  }

  if (counter == 3) {
    counter = 0;
  }

  for (int i = 0; i < strip->numPixels(); i = i + 3) {
    setPixel(i + counter,
             data->colors[i + counter]); // turn every third pixel on
  }
  show();
  counter++;
}

} // namespace leds