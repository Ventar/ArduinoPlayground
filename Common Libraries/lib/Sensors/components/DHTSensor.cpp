#include "DHTSensor.h"

#include <sensors/DHTHeatIndex.h>
#include <sensors/DHTHumidity.h>
#include <sensors/DHTTemperature.h>

namespace sensors {

void DHTSensor::setup() {
  this->dht = new DHT(pin, DHT22);
  dht->begin();

  sensors.reserve(3);

  sensors.push_back(new DHTTemperature(dht));
  sensors.push_back(new DHTHumidity(dht));
  sensors.push_back(new DHTHeatIndex(dht));
}

void DHTSensor::loop() {
  for (size_t i = 0; i < this->sensors.size(); i++) {
    sensors.at(i)->loop();
  }
}

String DHTSensor::getTemperature() { return sensors.at(0)->getValue(); }

String DHTSensor::getHumidity() { return sensors.at(1)->getValue(); }

String DHTSensor::getHeatIndex() { return sensors.at(2)->getValue(); }

} // namespace sensors