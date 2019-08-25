#ifndef HomeControl_Radar_h
#define HomeControl_Radar_h

#include "Component.h"
#include "SensorActive.h"

namespace sensors {

/**
 * Radar sensor of type RCWL-0561 to detect movement.
 */
class Radar : public SensorActive<Radar>, public Component {
 public:
  Radar(int pin) : SensorActive(MOVEMENT), Component("RCWL0561-" + String(pin)) { this->pin = pin; };

  /**
   *  Setup method used to initialize the component
   */
  void setup() override;

  /**
   * Logic executed during every loop() in the main programm.
   */
  void loop() override;

  /**
   * Returns the value of the sensor in a String representation.
   *
   * @return the value
   */
  String getValue() override;

  /**
   * Returns if movement was detected by this sensor or not.
   *
   * @return true if movement was detected, false otherwise
   */
  boolean isMovementDetected();

 private:
  /**
   * The pin to which the sensor is connected.
   */
  int pin;

  /**
   * Variable to hold information if movement was detected or not.
   */
  boolean movementDetected = false;
};

}  // namespace sensors

#endif