#include <ChartGenerator.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266httpUpdate.h>
#include <FS.h>

namespace chart {

ChartGenerator::ChartGenerator(sensors::DataCollector* collector) { this->data = collector; }

void ChartGenerator::loop() {
  if (millis() - lastGeneration > 60000) {
    WiFiClientSecure client;
    client.setInsecure();
    Serial.print("connecting to server");

    if (!client.connect("quickchart.io", 443)) {
      Serial.println("connection failed");
      return;
    }

    client.print(String("GET "
                        "https://quickchart.io/"
                        "chart?width=500&height=300&c={type:%27bar%27,data:{labels:[%"
                        "27January%27,%27February%27,%27March%27,%27April%27,%20%27May%27]"
                        ",%20datasets:[{label:%27Dogs%27,data:[50,60,70,180,190]},{label:%"
                        "27Cats%27,data:[100,200,300,400,500]}]}} HTTP/1.1\r\n") +
                 "Host: quickchart.io\r\n" + "User-Agent: ESP8266\r\n Connection: close\r\n\r\n");

    long len = -1;
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line.startsWith("Content-Length:")) {
        len = line.substring(15).toInt();
        Serial.printf("Content-Length: %d\n", len);
      }
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }

    File f = SPIFFS.open("chart.png", "w");
    if (f) {
      uint8_t buff[1024];
      while (client.connected() && (len > 0 || len == -1)) {
        size_t size = client.available();
        if (size > 0) {
          int c = client.readBytes(buff, ((size > 1024) ? 1024 : size));
          f.write(buff, c);
          if (len > 0) len -= c;
        } else {
          Serial.printf("No new bytes\n");
          continue;
        }
        Serial.printf("bytes left %d\n", len);
        ESP.wdtDisable();
        ESP.wdtEnable(1000);
      }
      Serial.println("done.");
      f.close();
    } else {
      Serial.println("Cannot store file");
    }

    lastGeneration = millis();
  }
}

}  // namespace chart