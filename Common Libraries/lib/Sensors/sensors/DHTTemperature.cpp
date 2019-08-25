#include "DHTTemperature.h"

namespace sensors {

void DHTTemperature::setup() { sensingDelay = 10000; }

String DHTTemperature::collectValue() {
  float temp = dht->readTemperature();
  return String(temp);
}

}  // namespace sensors
