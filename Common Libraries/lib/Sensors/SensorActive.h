#ifndef HomeControl_SensorActive_h
#define HomeControl_SensorActive_h

#include "Sensor.h"
#include <Arduino.h>
#include <list>

namespace sensors {

/** A passive sensor that updates the internal value on a regular base based on
 * a configured delay. */
template <class T> class SensorActive : public Sensor {
public:
  /**
   * Callback event that is triggered when the sensor was activated.
   */
  typedef std::function<void(T *sensor)> SensorTriggeredEvent;

  /**
   * Default constructor.
   *
   * @param metric the type of data sensed by the sensor.
   * @param dataType the data type of the sensor. Can be used in programms to
   * calculate values and decide if actions are triggered.
   */
  SensorActive(SensorMetric metric) : Sensor(ACTIVE, metric, CUSTOM){};

  /**
   * Registers a callback for this sensor that is triggered when the sensor was
   * activated / detected something.
   */
  void registerCallback(SensorTriggeredEvent callback) {
    this->callbacks.push_back(callback);
  }

protected:
  /**
   * Reference to the callback that is triggered when the sensor was activated.
   */
  std::list<SensorTriggeredEvent> callbacks;
};

} // namespace sensors

#endif
