#ifndef ChartGenerator_h
#define ChartGenerator_h

#include <Arduino.h>
#include <DataCollector.h>

namespace chart {

/**
 * Uses quickchart.io to generate a png and store it in the local file system.
 */
class ChartGenerator {
 public:
  /**
   * Constructor.
   *
   * @param collector
   *           the class with the data to be charted
   */
  ChartGenerator(sensors::DataCollector* collector);

  /**
   * Executes a request to generate a chart if the generation interval has passed.
   */
  void loop();

 private:
  /**
   * Reference to the sensor data that should be displayed.
   */
  sensors::DataCollector* data;

  /**
   * Last time a chart was generated.
   */
  uint64_t lastGeneration;
};

}  // namespace chart

#endif
