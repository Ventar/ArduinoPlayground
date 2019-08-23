#include <Arduino.h>
#include <CommonUtilities.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <FS.h> 

const String WIFI_NAME = "Destiny_EXT";
const String WIFI_PASSWORT = "8882941015907883";




  uint16_t httpsPort = 443;
const char * host = "raw.githubusercontent.com";//TOD: read from settings file
const char * fingerprint = "CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33";//TOD: read from settings file (can see in browser certificate info, MUST BE UPPERCASE!)

void updateFile(String url, String file) {
	WiFiClientSecure client;
  client.setInsecure();
	Serial.print("connecting to ");
	Serial.println(host);
	
  if (!client.connect(host, httpsPort)) {
		Serial.println("connection failed");
		return;
	}

	

	String action = url.substring(url.indexOf(host) + strlen(host)) + file;

	client.print(String("GET https://quickchart.io/chart?width=500&height=300&c={type:%27bar%27,") +
  "data:{labels:[%27January%27,%27February%27,%27March%27,%27April%27,%20%27May%27]," +
  "%20datasets:[{label:%27Dogs%27,data:[50,60,70,180,190]},{label:%27Cats%27," +
  "data:[100,200,300,400,500]}]}}" + " HTTP/1.1\r\n" +
		"Host: " + host + "\r\n" +
		"User-Agent: ESP8266\r\n" +
		"Connection: close\r\n\r\n");

	long len = -1;
	while (client.connected()) {
		String line = client.readStringUntil('\n');
		if (line.startsWith("Content-Length:")) {
			len = line.substring(15).toInt();
		}
		if (line == "\r") {
			Serial.println("headers received");
			break;
		}
	}

	Serial.printf("[HTTPS] GET: %s\n",action.c_str());
	File f = SPIFFS.open(file, "w");
	if (f) {
		uint8_t buff[128];
		while (client.connected() && (len > 0 || len == -1)) {
			size_t size = client.available();
			if (size>0) {
				int c = client.readBytes(buff, ((size > 128) ? 128 : size));
				f.write(buff, c);
				if (len > 0) 	len -= c;
			}
			else { break; }
			Serial.printf("bytes left %i", len);
			ESP.wdtDisable();
			ESP.wdtEnable(1000);
		}
		Serial.println("done.");
		f.close();
	}
}

void setup() {
CommonUtilities::Utilities::setupSerial("--          LED Matrix Display       --");
    CommonUtilities::Utilities::connectToWifi(WIFI_NAME, WIFI_PASSWORT);
}

void loop() {
  // put your main code here, to run repeatedly:
}