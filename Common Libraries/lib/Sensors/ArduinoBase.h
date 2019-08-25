#ifndef HomeControlComponent_h
#define HomeControlComponent_h

namespace sensors {

/**
 * Basic Component class for Arduino projects. The idea is that the two methods setup() and loop() which are usually
 * used in the main project are reflected by all custom components. This allows the user to hava a consistent view in
 * the main class to initialize them.
 */
class ArduinoBase {
 public:
  /**
   *  Setup method used to initialize the component
   */
  virtual void setup() = 0;

  /**
   * Logic executed during every loop() in the main programm.
   */
  virtual void loop() = 0;
};

}  // namespace sensors

#endif