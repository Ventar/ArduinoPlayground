#ifndef HomeControl_Device_h
#define HomeControl_Device_h

#include "Sensor.h"

namespace sensors {

/**
 * A component is a hardware part of the electronic setup that provides the home control with sensors and actors.
 */
class Component {
 public:
  /**
   * Default construcor
   *
   * @param name the name of this component to use it for rule creation in combination with the sensor metric type and
   * the hardware identifier.
   */
  Component(String name) { this->name = name; };

  /**
   * Returns a vector of sensors provided by this component.
   *
   * @return the sensors
   */
  std::vector<Sensor*> getSensors() { return sensors; };

  /**
   * Returns the first sensor of this component if available.
   *
   * @return the sensor
   */
  Sensor* getSensor() {
    if (sensors.size() > 0) {
      return sensors.at(0);
    } else {
      return NULL;
    }
  };

  /**
   * Returns the name of this component to use it for rule creation in combination with the sensor metric type and the
   * hardware identifier.
   *
   * @return the name
   */
  String getName() { return name; }

 protected:
  /** A vector of sensors. */
  std::vector<Sensor*> sensors;

 private:
  /**
   * The name of this component to use it for rule creation in combination with the sensor metric type and the
   * hardware identifier.
   */
  String name;
};

}  // namespace sensors

#endif