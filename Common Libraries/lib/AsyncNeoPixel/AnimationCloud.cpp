#include <AnimationCloud.h>
#include <algorithm>

namespace leds {

float modify(float c, float factor) { return c + (c * factor); }

void AnimationCloud::reset() {
  setAll(0, 0, 0);

  if (data->colors[0] == 0) {
    setColorListFromString("000044");
  }

  lastColor = 0;
}

void AnimationCloud::process() {
  float r, g, b, modificator;

  if (counter == 64) {
    counter = 0;
  }

  if (counter <= 32) {
    modificator = counter;
  } else if (counter <= 64) {
    modificator = (64 - counter);
  }

  for (uint16_t i = 0; i < strip->numPixels(); i++) {
    r = modify((data->colors[i] & 0x00ff0000UL) >> 16, std::cos(3.14 * i / 6)) +
        modificator;

    if (r < 0) {
      r = 0;
    } else if (r > 255) {
      r = 255;
    }

    g = modify((data->colors[i] & 0x0000ff00UL) >> 8, std::cos(3.14 * i / 6)) +
        modificator;

    if (g < 0) {
      g = 0;
    } else if (g > 255) {
      g = 255;
    }

    b = modify(data->colors[i] & 0x000000ffUL, std::cos(3.14 * i / 6)) +
        modificator;

    if (b < 0) {
      b = 0;
    } else if (b > 255) {
      b = 255;
    }

    setPixel(i, r, g, b);
  }

  counter++;

  uint32_t start = data->colors[0];

  for (uint16_t i = 0; i < strip->numPixels() - 1; i++) {
    data->colors[i] = data->colors[i + 1];
  }

  data->colors[strip->numPixels() - 1] = start;

  show();
}

} // namespace leds
