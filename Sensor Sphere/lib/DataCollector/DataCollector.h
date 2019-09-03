#ifndef DataCollector_h
#define DataCollector_h

#include <Arduino.h>
#include <WiFiClock.h>
#include <components/DHTSensor.h>
#include <components/Photoresistor.h>
#include <components/Radar.h>
#include <list>

namespace sensors {

/**
 * Dataset that contains data of the sensor sphere to create historical information.
 */
struct DataSet {
  DataSet(){};

  ~DataSet() { delete timestamp; }

  String* timestamp;
  float temperature;
  float humidity;
  float heatIndex;
  boolean movement;
  float light;

  String toString() {
    String s = String(*timestamp);
    s += " / temp. : " + String(temperature);
    s += " / hum.  : " + String(humidity);
    s += " / heat. : " + String(heatIndex);
    s += " / light.: " + String(light);
    s += " / movm. : " + String(movement);
    return s;
  };
};

/**
 * Simple in memory list that keeps sensor values for a certain period of time.
 */
class DataCollector {
 public:
  /**
   * Creates a new collector with the given sensors. NULL values are allowed.
   *
   * @param dht
   *          Temperature, humidity and heat index sensor.
   * @param photo
   *          Light sensor
   * @param radar
   *          Movement sensor
   * @param recordCreationInterval
   *          interval in ms in which new records are created
   * @param maximumNumberOfRecords maximum number of ecord kept in the collector. If the maximum number is reached the
   *          oldes record will be deleted
   */
  DataCollector(WiFiClock* clock, DHTSensor* dht, Radar* radar, Photoresistor* photo, uint64_t recordCreationInterval,
                uint16_t maximumNumberOfRecords);

  /**
   * Logic to execute during every loop() in the main programm.
   */
  void loop(uint64_t now);

  /**
   * Returns all available records.
   */
  std::list<DataSet*> getRecords();

 private:
  /**
   * Creates a new record in the collection.
   */
  void createRecord();

  /**
   * The collected data.
   */
  std::list<DataSet*> data;

  /**
   * Maximum number of records that are stored.
   */
  uint16_t maximumRecords = 5;

  /**
   * Last time a record was created.
   */
  uint64_t lastRecordCreationTime;

  /**
   * How often a new record is created in ms.
   */
  uint64_t recordCreationInterval = 60000;
  /**
   * Clock to measure the time when the data of the sensors was collected
   */
  WiFiClock* clock;
  /**
   * Temperature, humidity and heat index sensor.
   */
  DHTSensor* dht;

  /**
   * Light sensor
   */
  Photoresistor* photo;

  /**
   * Movement sensor
   */
  Radar* radar;
  /**
   * Flag to keep track of movements that happen during the interval.
   */
  boolean movementDetected;
};

}  // namespace sensors

#endif