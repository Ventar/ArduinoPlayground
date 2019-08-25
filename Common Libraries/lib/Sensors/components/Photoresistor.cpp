#include "Photoresistor.h"

namespace sensors {

void Photoresistor::setup() {
  sensingDelay = 10000;
  this->sensors.reserve(1);
  this->sensors.push_back(this);
}

String Photoresistor::collectValue() {
  int light = analogRead(this->pin);
  float percent = light * 100 / 1024.0;
  return String(percent);
}

}  // namespace sensors