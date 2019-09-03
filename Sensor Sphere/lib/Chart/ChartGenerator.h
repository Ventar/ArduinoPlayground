#ifndef ChartGenerator_h
#define ChartGenerator_h

#include <Arduino.h>
#include <DataCollector.h>
#include <base64.h>

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
  ChartGenerator(uint64_t renewalInterval, sensors::DataCollector* collector);

  /**
   * Executes a request to generate a chart if the generation interval has passed.
   */
  void loop(uint64_t now);

 private:
  /**
   * Reference to the sensor data that should be displayed.
   */
  sensors::DataCollector* collector;

  /**
   * Last time a chart was generated.
   */
  uint64_t lastGeneration;

  /**
   * Interval in which the quickchart.io endpoint is called to generate new data.
   */
  uint64_t renewalInterval = 60000;
};

}  // namespace chart

#endif
