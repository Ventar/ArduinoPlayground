#ifndef HomeControl_DHT22_h
#define HomeControl_DHT22_h

#include "Component.h"
#include <DHT.h>

namespace sensors {

/**
 * DHT22 sensor.
 */
class DHTSensor : public Component, public ArduinoBase {
public:
  /**
   * Default constructor.
   *
   * @param pin the PIN to which the sensor is connected.
   */
  DHTSensor(int pin) : Component("DHT22-" + String(pin)) { this->pin = pin; };

  /**
   * Setup method used to initialize the component
   */
  void setup() override;

  /**
   * Logic executed during every loop() in the main programm.
   */
  void loop() override;

  /**
   * Returns the temperature of this sensor.
   */
  String getTemperature();

  /**
   * Returns the humidity of this sensor.
   */
  String getHumidity();

  /**
   * Returns the heat index of this sensor.
   */
  String getHeatIndex();

private:
  /**
   * The pin to which the sensor is connected.
   */
  int pin;

  /**
   * Reference to the DHT lib for the sensor
   * */
  DHT *dht;
};

} // namespace sensors

#endif