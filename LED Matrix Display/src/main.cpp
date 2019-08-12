#include <Arduino.h>
#include <LEDMatrixDisplay.h>
#include <WiFiClock.h>

LEDMatrixDisplay display;
WiFiClock wifiClock;

/**
 * Connect to the WiFi network
 */
void connectToWifi(String accessPointName, String accessPointPassword) {
    Serial.printf("Connect to WiFi (MAC: %s)\n", WiFi.macAddress().c_str());

    WiFi.begin(accessPointName, accessPointPassword);

    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.printf("Connected (IP address: %s)\n\n", WiFi.localIP().toString().c_str());
}

/**
 * Setup the serial interface to debug.
 */
void setupSerial() {
    // setup the serial communication
    Serial.begin(115200);

    while (!Serial) {
        delay(50);
    }

    // ensure that we start with a clear line.
    Serial.println("\n");

    Serial.println("---------------------------------------");
    Serial.println("--             Circuit WS            --");
    Serial.println("---------------------------------------\n");
}

void setup() {
    setupSerial();
    connectToWifi("Destiny_EXT", "8882941015907883");

    display.setup();

    // wifiClock.setProxy("proxy.cycos.com", 8080);
    wifiClock.onUpdate([](String time) {
        // if (showClock) {
        display.setText(time, PA_CENTER, PA_PRINT, PA_NO_EFFECT);
        //}
    });

    wifiClock.setBlinkingColon(true);
    wifiClock.setup();
    pinMode(D4, INPUT);
    pinMode(D3, INPUT);
    pinMode(D2, INPUT);
    pinMode(D1, INPUT);
}

uint64_t lastPrint = millis();

void loop() {
    wifiClock.loop();
    display.loop();

    int d1 = digitalRead(D1);
    int d2 = digitalRead(D2);
    int d3 = digitalRead(D3);
    int d4 = digitalRead(D4);
    if (millis() - lastPrint > 1000) {
        Serial.printf("d1: %d / d2: %d / d3: %d / d4: %d\n", d1, d2, d3, d4);
        lastPrint = millis();
    }
}