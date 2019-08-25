#ifndef HomeControl_SensorPassive_h
#define HomeControl_SensorPassive_h

#include <Arduino.h>
#include "Sensor.h"

namespace sensors {

/** A passive sensor that updates the internal value on a regular base based on a configured delay. */
class SensorPassive : public Sensor {
 public:
  /**
   * Default constructor.
   *
   * @param metric the type of data sensed by the sensor.
   * @param dataType the data type of the sensor. Can be used in programms to calculate values and decide if actions
   *     are triggered.
   */
  SensorPassive(SensorMetric metric, SensorDataType dataType, String unit) : Sensor(PASSIVE, metric, dataType, unit){};

  /**
   * Logic executed during every loop() in the main programm.
   */
  void loop() override;

  /**
   * Returns the unit of the sensor for display purposes.
   *
   * @return the unit
   */
  String getValue() override;

 protected:
  /** Collect the value from the underlying sensor
   *
   * @return the value
   */
  virtual String collectValue() = 0;

  /**
   * The delay of the sensor. During the processing of he loop method the value of the sensor is only updated if the
   * delay
   */
  unsigned int sensingDelay = 0;

 private:
  /**
   * The last recognized value of the sensor.
   */
  String value = "n/a";

  /** The last time this sensor was read. In combination with the delay this will update the collected value */
  unsigned long last = 0;
};

}  // namespace sensors

#endif
