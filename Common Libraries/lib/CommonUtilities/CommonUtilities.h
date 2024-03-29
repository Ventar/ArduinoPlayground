#ifndef CommonUtilities_h
#define CommonUtilities_h

#include <Arduino.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

namespace CommonUtilities {

class Utilities {
public:
  /**
   * Setup the serial interface to debug.
   */
  static void setupSerial(String name) {
    // setup the serial communication
    Serial.begin(115200);

    while (!Serial) {
      delay(50);
    }

    // ensure that we start with a clear line.
    Serial.println("\n");

    Serial.println("---------------------------------------");
    Serial.println(name);
    Serial.println("---------------------------------------\n");
    Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
  }

  /**
   * Connect to the WiFi network
   */
  static void connectToWifi(String accessPointName,
                            String accessPointPassword) {
    Serial.printf("Connect to WiFi (MAC: %s)\n", WiFi.macAddress().c_str());

    WiFi.begin(accessPointName, accessPointPassword);

    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println();
    Serial.printf("Connected (IP address: %s)\n\n",
                  WiFi.localIP().toString().c_str());
  }
};

} // namespace CommonUtilities

#endif
