#ifndef HomeControlSensor_h
#define HomeControlSensor_h

#include <Arduino.h>
#include "ArduinoBase.h"

namespace sensors {

/** The basice type of the sensor. */
enum SensorType {
  /**
   * Active sensors react on data transmitted or recognized by them, e.g. an IR receiver or the RCWL-0561 radar
   * module.
   */
  ACTIVE,

  /**
   * Passive sensors are not triggered by an external source but sense on a continues base.
   */
  PASSIVE
};

/**
 * The metric measured by the sensor.
 */
enum SensorMetric { TEMPERATURE, HUMIDITY, HEAT_INDEX, MOVEMENT, TIME, LIGHT, BUTTON };

/**
 * The datatype provided by the sensor.
 */
enum SensorDataType { FLOAT, INT, PERCENT, STRING, CUSTOM };

/**
 * The basic sensor class. For the home controll project there is a differentiation between sensors which
 * collect or calculate values and actors which execute actions. A sensor is not strictly limited to a physical device
 * here it is more a definition of a programming element that can be used in scripts to trigger actors. An example is
 * the time sensor which is not a hardware device but uses the WiFi capability of the board to connect to the internet
 * and take the current time.
 */
class Sensor : public ArduinoBase {
 public:
  /**
   * Default constructor.
   *
   * @param type the sensor type
   * @param metric the type of data sensed by the sensor.
   * @param dataType the data type of the sensor. Can be used in programms to calculate values and decide if actions
   *     are triggered.
   */
  Sensor(SensorType type, SensorMetric metric, SensorDataType dataType, String unit = "");

  /**
   * Returns the type of this sensor.
   *
   * @return the type.
   */
  SensorType getSensorType();

  /**
   * Returns the type of data sensed by the sensor.
   *
   * @return the metric
   */
  SensorMetric getSensorMetric();

  /**
   * Returns the sensor data type.
   *
   * @return the data type
   */
  SensorDataType getSensorDataType();

  /**
   * Returns the value of the sensor in a String representation.
   *
   * @return the value
   */
  virtual String getValue() = 0;

  /**
   * Returns the unit of the sensor for display purposes.
   *
   * @return the unit
   */
  String getUnit();

  /**
   * Converts the sensor value to a String value with additional information.
   */
  virtual String toString();

  /**
   * Converts the sensor value to a String value with additional information.
   * @param a preficx for the string
   */
  virtual String toString(String prefix);

  /**
   * Activates or deaktivates this sensor.
   *
   * @param active if the sensor should be active or not.
   */
  void setActive(boolean active);

  /**
   * Returns if the sensor is active or not.
   *
   * @return true if the sensor is active, false otherwise.
   */
  boolean isActive();

 protected:
  /**
   * The type of the sensor.
   */
  SensorType type;

  /**
   * The type of data that is collected by the sensor
   */
  SensorMetric metric;

  /**
   * The data type of the sensor. Can be used in programms to calculate values and decide if actions are triggered.
   */
  SensorDataType dataType;

  /**
   * Boolean value that indicates if this sensor is active or not.
   */
  boolean active = true;

  /**
   * The measurement unit of the sensor.
   */
  String unit;
};

}  // namespace sensors

#endif