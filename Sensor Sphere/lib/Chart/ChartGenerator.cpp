#include <ChartGenerator.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266httpUpdate.h>
#include <FS.h>

namespace chart {

ChartGenerator::ChartGenerator(uint64_t renewalInterval, sensors::DataCollector* collector) {
  this->renewalInterval = renewalInterval;
  this->collector = collector;
}

void ChartGenerator::loop(uint64_t now) {
  if (now - lastGeneration > renewalInterval) {
    WiFiClientSecure client;
    client.setInsecure();
    Serial.print("connecting to server");

    if (!client.connect("quickchart.io", 443)) {
      Serial.println("connection failed");
      return;
    }

    String time = "";
    String data = "";

    for (sensors::DataSet* rec : collector->getRecords()) {
      time += "'" + rec->timestamp->substring(11, 16) + "',";
      data += String(rec->temperature) + ",";
    }
    time = time.substring(0, time.length() - 1);
    data = data.substring(0, data.length() - 1);

    String quickchartTemplate =
        base64::encode("{type: 'line', data: { labels: [" + time +
                       "], datasets: [{ label: "
                       "'Temperature',  "
                       " data: [ " +
                       data +
                       " ] }]}, options: {legend: { position: 'bottom' }, plugins: { datalabels: { "
                       "display: true, font: { style: 'italic', }}}}}");
    quickchartTemplate.replace("\n", "");

    client.print(String("GET "
                        "https://quickchart.io/"
                        "chart?width=500&height=300&encoding=base64&backgroundColor=white&c=" +
                        quickchartTemplate + " HTTP/1.1\r\n") +
                 "Host: quickchart.io\r\n" + "User-Agent: ESP8266\r\n Connection: close\r\n\r\n");

    long len = -1;
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line.startsWith("Content-Length:")) {
        len = line.substring(15).toInt();
        Serial.printf("Content-Length: %d\n", (int)len);
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
          continue;
        }
        Serial.printf("bytes left %d\n", len);
      }
      Serial.println("done.");
      f.close();
    } else {
      Serial.println("Cannot store file");
    }

    client.stop();
    lastGeneration = millis();
  }
}

}  // namespace chart