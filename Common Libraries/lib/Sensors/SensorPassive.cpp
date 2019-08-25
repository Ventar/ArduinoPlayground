#include "SensorPassive.h"

namespace sensors {

String SensorPassive::getValue() {
  if (active) {
    return value;
  } else {
    return "DISABLED";
  }
}

void SensorPassive::loop() {
  unsigned long now = millis();

  if (now - last > sensingDelay) {
    last = millis();
    value = collectValue();
  }
}

}  // namespace sensors