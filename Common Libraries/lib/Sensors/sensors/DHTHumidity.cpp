#include "DHTHumidity.h"

namespace sensors {

void DHTHumidity::setup() { sensingDelay = 10000; }

String DHTHumidity::collectValue() {
  float temp = dht->readHumidity();
  return String(temp);
}

}  // namespace sensors
