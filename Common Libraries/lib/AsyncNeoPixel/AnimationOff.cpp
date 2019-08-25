#include <AnimationOff.h>

namespace leds {

void AnimationOff::reset() {
  setAll(0, 0, 0);
  show();
}

void AnimationOff::process() {}

} // namespace leds