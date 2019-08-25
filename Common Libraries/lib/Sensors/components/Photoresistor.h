#ifndef HomeControl_Photoresistor_h
#define HomeControl_Photoresistor_h

#include "Component.h"
#include "SensorPassive.h"

namespace sensors {

/**
 * Simple photo resistor to measure light intensity in a range from 0 to 1024.
 */
class Photoresistor : public SensorPassive, public Component {
 public:
  Photoresistor(int pin) : SensorPassive(LIGHT, FLOAT, "%"), Component("PHOTORESISTOR-" + String(pin)) {
    this->pin = pin;
  };

  /** Setup method used to initialize the component */
  void setup() override;

 protected:
  /** Collect the value from the underlying sensor
   *
   * @return the value
   */
  String collectValue() override;

 private:
  /**
   * The pin to which the sensor is connected.
   */
  int pin;
};

}  // namespace sensors

#endif