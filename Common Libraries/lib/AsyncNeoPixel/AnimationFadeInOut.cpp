#include <AnimationFadeInOut.h>

namespace leds {

void AnimationFadeInOut::reset() {
  setAll(0, 0, 0);

  if (data->colors[0] == 0) {
    setColorListFromString("000044");
  }

  counter = 0;
  mode = UP;
}

void AnimationFadeInOut::process() {
  float r, g, b;

  for (uint16_t i = 0; i < strip->numPixels(); i++) {
    r = (counter / 256.0) * ((data->colors[i] & 0x00ff0000UL) >> 16);
    g = (counter / 256.0) * ((data->colors[i] & 0x0000ff00UL) >> 8);
    b = (counter / 256.0) * (data->colors[i] & 0x000000ffUL);
    setPixel(i, r, g, b);
  }

  show();

  if (mode == UP) {
    counter++;

    if (counter == 255) {
      mode = DOWN;
      counter = 255;
    }
  } else {
    counter -= 1;
    if (counter == 0) {
      mode = UP;
    }
  }
}

} // namespace leds