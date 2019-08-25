#include "DHTHeatIndex.h"

namespace sensors {

void DHTHeatIndex::setup() { sensingDelay = 10000; }

String DHTHeatIndex::collectValue() {
  float temp = dht->computeHeatIndex(dht->readTemperature(), dht->readHumidity(), false);
  return String(temp);
}

}  // namespace sensors
