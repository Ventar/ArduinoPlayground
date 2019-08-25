#include <Arduino.h>

#include <AsyncNeoPixel.h>
#include <ChartGenerator.h>
#include <CommonUtilities.h>
#include <DataCollector.h>
#include <FS.h>
#include <WebServer.h>
#include <WiFiClock.h>
#include <components/DHTSensor.h>
#include <components/Photoresistor.h>
#include <components/Radar.h>
#include <list>

const String WIFI_NAME = "Destiny_EXT";
const String WIFI_PASSWORT = "8882941015907883";

leds::AsyncNeoPixel strip(14, D5, NEO_GRB + NEO_KHZ800);

WiFiClock wifiClock;

sensors::DHTSensor dht(D7);
sensors::Photoresistor photo(A0);
sensors::Radar radar(D0);

sensors::DataCollector dataCollector(&wifiClock, &dht, &radar, &photo, 5000, 5);
chart::ChartGenerator chartGen(&dataCollector);

webserver::WebServer server(80);

bool enableRadar = false;
leds::AsyncNeoPixelMode currentMode = leds::AsyncNeoPixelMode::OFF;
unsigned long last = millis();

void setup() {
  SPIFFS.begin();

  strip.setup();
  strip.setMode(leds::AsyncNeoPixelMode::OFF);

  CommonUtilities::Utilities::setupSerial("--             Sensor Sphere         --");
  CommonUtilities::Utilities::connectToWifi(WIFI_NAME, WIFI_PASSWORT);

  wifiClock.setup();
  wifiClock.setBlinkingColon(false);

  dht.setup();
  photo.setup();
  radar.setup();
  server.setup();

  server.onHttpRequest("/csv", [](ESP8266WebServer* server) {
    String csv = "time;temperature;humidity;heatindex;light;movement\n";

    for (sensors::DataSet* rec : dataCollector.getRecords()) {
      csv = csv + *rec->timestamp + ";" + String(rec->temperature) + ";" + String(rec->humidity) + ";" +
            String(rec->heatIndex) + ";" + String(rec->light) + ";" + (rec->movement ? "true" : "false") + "\n";
    }

    server->send(200, "text/plain", csv);
  });

  server.onHttpRequest("/json", [](ESP8266WebServer* server) {
    DynamicJsonDocument doc(4196);
    JsonArray data = doc.createNestedArray("data");

    for (sensors::DataSet* rec : dataCollector.getRecords()) {
      JsonObject jRec = data.createNestedObject();
      jRec["timestamp"] = *rec->timestamp;
      jRec["temperature"] = rec->temperature;
      jRec["humidity"] = rec->humidity;
      jRec["heatIndex"] = rec->heatIndex;
      jRec["light"] = rec->light;
      jRec["movement"] = rec->movement;
    }

    String output = "";
    serializeJson(doc, output);

    server->send(200, "text/plain", output);
  });

  server.onHttpRequest("/radar/enable", [](ESP8266WebServer* server) {
    enableRadar = true;
    server->send(200, "text/plain", "ok");
  });

  server.onHttpRequest("/radar/disable", [](ESP8266WebServer* server) {
    enableRadar = false;
    strip.setMode(leds::AsyncNeoPixelMode::OFF);
    currentMode = leds::AsyncNeoPixelMode::OFF;
    server->send(200, "text/plain", "ok");
  });

  server.onHttpRequest("/chart/download", [](ESP8266WebServer* server) {
    File dataFile = SPIFFS.open("chart.png", "r");

    Serial.printf("file size %d\n", dataFile.size());

    server->setContentLength(dataFile.size());
    server->sendHeader("Connection", "keep-alive");
    server->send(200, "image/png", "");

    WiFiClient client = server->client();
    client.setNoDelay(true);

    char buf[1024];
    int dataToSend = dataFile.size();
    while (dataToSend > 0) {
      size_t len = std::min((int)(sizeof(buf) - 1), dataToSend);
      dataFile.readBytes(buf, len);
      client.write(buf, len);
      client.flush();
      dataToSend -= len;
    }

    dataFile.close();
  });

  radar.registerCallback([](sensors::Radar* r) {
    if (enableRadar) {
      strip.setMode(leds::AsyncNeoPixelMode::THEATER_CHASE,
                    "ff0000,0000ff,ff0000,0000ff,ff0000,0000ff,ff0000,0000ff,ff0000,0000ff,ff0000,0000ff,ff0000,0000ff",
                    "95");
      currentMode = leds::AsyncNeoPixelMode::THEATER_CHASE;
      last = millis();
    }
  });
  // strip.setMode(leds::AsyncNeoPixelMode::STATIC, "004400");
}

void loop() {
  strip.loop();
  wifiClock.loop();
  dht.loop();
  photo.loop();
  radar.loop();
  dataCollector.loop();
  server.loop();
  chartGen.loop();

  if (millis() - last > 10000) {
    last = millis();
    if (enableRadar && !radar.isMovementDetected() && currentMode == leds::AsyncNeoPixelMode::THEATER_CHASE) {
      strip.setMode(leds::AsyncNeoPixelMode::OFF);
      currentMode = leds::AsyncNeoPixelMode::OFF;
    } else if (!enableRadar && currentMode != leds::AsyncNeoPixelMode::OFF) {
      strip.setMode(leds::AsyncNeoPixelMode::OFF);
      currentMode = leds::AsyncNeoPixelMode::OFF;
    }
  }
}