#include "Radar.h"

namespace sensors {

void Radar::setup() {
  pinMode(pin, INPUT);
  this->sensors.reserve(1);
  this->sensors.push_back(this);
}

void Radar::loop() {
  if (active) {
    boolean oldValue = movementDetected;
    movementDetected = digitalRead(pin);

    if (oldValue != movementDetected && !callbacks.empty()) {
      for (SensorTriggeredEvent callback : callbacks) {
        callback(this);
      }
    }
  }
}

boolean Radar::isMovementDetected() { return movementDetected; }

String Radar::getValue() {
  if (active) {
    if (movementDetected) {
      return "MOVEMENT_DETECTED";
    } else {
      return "NOTHING_DETECTED";
    }
  } else {
    return "DISABLED";
  }
}

} // namespace sensors