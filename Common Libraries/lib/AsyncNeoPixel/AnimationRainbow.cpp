#include <AnimationRainbow.h>

namespace leds {

uint8_t *AnimationRainbow::wheel(uint8_t WheelPos) {
  static uint8_t c[3];

  if (WheelPos < 85) {
    c[0] = WheelPos * 3;
    c[1] = 255 - WheelPos * 3;
    c[2] = 0;
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    c[0] = 255 - WheelPos * 3;
    c[1] = 0;
    c[2] = WheelPos * 3;
  } else {
    WheelPos -= 170;
    c[0] = 0;
    c[1] = WheelPos * 3;
    c[2] = 255 - WheelPos * 3;
  }

  return c;
}

void AnimationRainbow::process() {
  uint8_t *c;

  counter++;
  for (uint16_t i = 0; i < strip->numPixels(); i++) {
    c = wheel(((i * 256 / strip->numPixels()) + counter - 1) & 255);
    setPixel(i, *c, *(c + 1), *(c + 2));
  }
  show();

  if (counter > 255) {
    counter = 0;
  }
}

} // namespace leds