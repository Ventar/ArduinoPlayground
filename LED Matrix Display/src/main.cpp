#include <Arduino.h>
#include <CircuitClient.h>
#include <LEDMatrixDisplay.h>
#include <PresenceState.h>
#include <TextItem.h>
#include <WiFiClock.h>

// const String WIFI_NAME = "MRO Finger Weg";
// const String WIFI_PASSWORT = "7b559b98adc2";

const String WIFI_NAME = "Destiny_EXT";
const String WIFI_PASSWORT = "8882941015907883";

const String CIRCUIT_SYSTEM = "circuitsandbox.net";
const String CIRCUIT_USER = "michael.rodenbuecher@arduino.com";
const String CIRCUIT_PWD = "sdf56JKL!";

Circuit::CircuitClient* circuit;
LEDMatrixDisplay display;
WiFiClock wifiClock;

enum ActivePage { CLOCK, NEW_MESSAGES, INCOMING_CALL, DISPLAY_EVENT };

bool shortMessageCounter = false;
int newMessages = 0;
ActivePage page = CLOCK;

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
    Serial.println("--          LED Matrix Display       --");
    Serial.println("---------------------------------------\n");
}

/**
 * Registers a callback in the circuit client to get informed about new messages.
 */
void registerNewMessageCallback() {
    circuit->onConversationAddItemEvent([](Circuit::TextItem item) {
        newMessages++;
        page = DISPLAY_EVENT;
        display.setTextWithCallback(" New Message ", PA_RIGHT, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT, []() {
            display.setText(wifiClock.getTime(), PA_CENTER, PA_PRINT, PA_NO_EFFECT, false, false);
            page = CLOCK;
        });
    });
}

void setup() {
    setupSerial();

    connectToWifi(WIFI_NAME, WIFI_PASSWORT);

    display.setup();

    // wifiClock.setProxy("proxy.cycos.com", 8080);
    wifiClock.onUpdate([](String time) {
        if (page == CLOCK) {
            display.setText(time, PA_CENTER, PA_PRINT, PA_NO_EFFECT);
        }
    });

    wifiClock.setBlinkingColon(true);
    wifiClock.setup();

    circuit = new Circuit::CircuitClient("circuitsandbox.net", 443, "michael.rodenbuecher@arduino.com", "sdf56JKL!");
    circuit->setup();

    registerNewMessageCallback();

    pinMode(D4, INPUT);
    pinMode(D3, INPUT);
    pinMode(D2, INPUT);
    pinMode(D1, INPUT);
}

uint64_t lastPrint = millis();

void displayNewMessages() {
    page = NEW_MESSAGES;

    if (shortMessageCounter) {
        display.setText("< " + String(newMessages) + " >", PA_CENTER, PA_PRINT, PA_NO_EFFECT, true, false, false);
    } else {
        if (newMessages == 0) {
            display.setText("No Messages ", PA_RIGHT, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT, true);
        } else if (newMessages == 1) {
            display.setText(String(newMessages) + " new Message ", PA_RIGHT, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT, true);
        } else {
            display.setText(String(newMessages) + " new Messages ", PA_RIGHT, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT, true);
        }
    }
}

void loop() {
    wifiClock.loop();
    display.loop();
    circuit->loop();

    int d1 = digitalRead(D1);
    int d2 = digitalRead(D2);
    int d3 = digitalRead(D3);
    int d4 = digitalRead(D4);

    if (d1 == 0) {
        page = CLOCK;
        display.setText(wifiClock.getTime(), PA_CENTER, PA_PRINT, PA_NO_EFFECT);
        delay(100);
    }

    if (d2 == 0) {
        displayNewMessages();
        delay(100);
    }

    if (d3 == 0) {
        if (page == NEW_MESSAGES) {
            shortMessageCounter = !shortMessageCounter;
            displayNewMessages();
            delay(100);
        }
    }

    /*
    if (millis() - lastPrint > 1000) {
        Serial.printf("d1: %d / d2: %d / d3: %d / d4: %d\n", d1, d2, d3, d4);
        lastPrint = millis();
    }
    */
}
