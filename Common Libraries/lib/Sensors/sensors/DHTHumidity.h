#ifndef HomeControl_SensorHumidity_h
#define HomeControl_SensorHumidity_h

#include <Arduino.h>
#include <DHT.h>
#include "SensorPassive.h"

namespace sensors {

/**
 * Humidity Sensor based on the DHT22 module.
 */
class DHTHumidity : public SensorPassive {
 public:
  /**
   * Default constructor
   *
   * @param DHT the reference to the device. The reference is needed here because the device will offer three
   * different logical sensors (temperature, humidity, heatIndex) from the perspective of the implmenetation logic of
   * the programm.
   */
  DHTHumidity(DHT* dht) : SensorPassive(HUMIDITY, FLOAT, "%") { this->dht = dht; };

  /** Setup method used to initialize the component */
  void setup() override;

  /** Collect the value from the underlying sensor
   *
   * @return the value
   */
  String collectValue() override;

 private:
  /**
   * The sensor device.
   */
  DHT* dht;
};

}  // namespace sensors

#endif
