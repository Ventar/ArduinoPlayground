#include "DataCollector.h"

namespace sensors {

DataCollector::DataCollector(WiFiClock* clock, DHTSensor* dht, Radar* radar, Photoresistor* photo,
                             uint64_t recordCreationInterval, uint16_t maximumNumberOfRecords) {
  this->clock = clock;
  this->dht = dht;
  this->radar = radar;
  this->photo = photo;
  this->recordCreationInterval = recordCreationInterval;
  this->maximumRecords = maximumNumberOfRecords;

  if (radar != NULL) {
    radar->registerCallback([this](Radar* r) { this->movementDetected = r->isMovementDetected(); });
  }

  this->lastRecordCreationTime = millis();
}

void DataCollector::loop(uint64_t now) {
  if (now - lastRecordCreationTime > recordCreationInterval) {
    createRecord();
    lastRecordCreationTime = millis();
    this->movementDetected = false;
  }
}

void DataCollector::createRecord() {
  DataSet* dSet = new DataSet();
  dSet->timestamp = new String(clock->getISO8601());

  if (dht != NULL) {
    dSet->temperature = dht->getTemperature().toFloat();
    dSet->humidity = dht->getHumidity().toFloat();
    dSet->heatIndex = dht->getHeatIndex().toFloat();
  }

  if (radar != NULL) {
    dSet->movement = this->movementDetected;
  }

  if (photo != NULL) {
    dSet->light = photo->getValue().toFloat();
  }

  data.push_back(dSet);

  Serial.printf("Add data ::= [%s] , size ::= [%d]\n", dSet->toString().c_str(), data.size());
  Serial.printf("Free heap: %d\n", ESP.getFreeHeap());

  if (data.size() > maximumRecords) {
    DataSet* first = data.front();
    data.pop_front();
    delete first;
  }
}

std::list<DataSet*> DataCollector::getRecords() { return data; }

}  // namespace sensors
