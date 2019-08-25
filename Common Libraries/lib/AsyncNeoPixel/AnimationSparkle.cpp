#include <AnimationSparkle.h>

namespace leds {

void AnimationSparkle::reset() {
  setAll(0, 0, 0);

  if (data->colors[0] == 0) {
    setColorListFromString("000044");
  }

  for (uint8_t i = 0; i < 45; i++) {
    memory[i] = 0;
  }

  counter = 0;
}

void AnimationSparkle::process() {
  if (counter < 45) {
    uint16_t ledToSet = random(strip->numPixels());
    setPixel(ledToSet, data->colors[ledToSet]);
    memory[counter] = ledToSet;
    counter++;
    show();
  } else if (counter < 90) {
    setPixel(memory[counter - 45], 0);
    counter++;
    show();
  } else {
    counter = 0;
  }
}

} // namespace leds