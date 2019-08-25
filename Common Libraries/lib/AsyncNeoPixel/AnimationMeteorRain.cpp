#include <AnimationMeteorRain.h>
#include <Arduino.h>

namespace leds {

void AnimationMeteorRain::reset() {
  setAll(0, 0, 0);
  if (data->colors[0] == 0) {
    setColorListFromString("000044");
  }
  counter = 0;
}

void AnimationMeteorRain::setMeteorDecay(uint8_t decay) {
  this->meteorTrailDecay = decay;
};

void AnimationMeteorRain::setMeteorSize(uint8_t size) {
  this->meteorSize = size;
};

void AnimationMeteorRain::setMeteorRandomDecay(boolean random) {
  this->meteorRandomDecay = random;
};

void AnimationMeteorRain::process() {
  if (counter == strip->numPixels() * 2) {
    setAll(0, 0, 0);
    counter = 0;
  } else {
    // fade brightness all LEDs one step
    for (int j = 0; j < strip->numPixels(); j++) {
      if ((!meteorRandomDecay) || (random(10) > 5)) {
        uint32_t oldColor;
        uint8_t r, g, b;

        oldColor = strip->getPixelColor(j);
        r = (oldColor & 0x00ff0000UL) >> 16;
        g = (oldColor & 0x0000ff00UL) >> 8;
        b = (oldColor & 0x000000ffUL);

        r = (r <= 10) ? 0 : (int)(r - (r * meteorTrailDecay / 256));
        g = (g <= 10) ? 0 : (int)(g - (g * meteorTrailDecay / 256));
        b = (b <= 10) ? 0 : (int)(b - (b * meteorTrailDecay / 256));

        strip->setPixelColor(j, r, g, b);
      }
    }

    // draw meteor
    for (int j = 0; j < meteorSize; j++) {
      if ((counter - j < strip->numPixels()) && (counter - j >= 0)) {
        setPixel(counter - j, data->colors[0]);
      }
    }

    show();
    counter++;
  }
}

} // namespace leds