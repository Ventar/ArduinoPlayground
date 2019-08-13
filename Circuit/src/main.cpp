#include <Arduino.h>
#include <ArduinoJson.h>
#include <CircuitClient.h>
#include <ESP8266WiFi.h>
#include <PresenceState.h>
#include <TextItem.h>

//#define DEBUG_CIRCUIT

const String WIFI_NAME = "Destiny_EXT";
const String WIFI_PASSWORT = "8882941015907883";

Circuit::CircuitClient* circuit;

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

bool answered = false;

void setup() {
    setupSerial();                            // Setup the serial interface to debug.
    connectToWifi(WIFI_NAME, WIFI_PASSWORT);  // connect to the WiFi network
    //circuit = new Circuit::CircuitClient("192.168.2.110", 443, "michael.rodenbuecher@atos.net", "sdf56JKL!");
    circuit = new Circuit::CircuitClient("circuitsandbox.net", 443, "michael.rodenbuecher@unify.com", "sdf56JKL!");
    circuit->onConversationAddItemEvent([](Circuit::TextItem item) {
        Serial.printf("[Main] New TextItem received ::= [%s]\n", item.toString().c_str());
        if (!answered) {
            answered = true;
            String content = String("Reply to: ") + item.content;
            circuit->post(item.conversationId, content, "", [](Circuit::ResultCode resultCode, Circuit::TextItem item) {
                Serial.printf("[Main] Response for new text item creation ::= [%s]\n", item.toString().c_str());
            });
        }
    });

    circuit->onUserUserPresenceChangedEvent([](Circuit::PresenceState state) {
            Serial.printf("[Main] Presence state of user ::= [%s] is ::= [%s]\n", state.userId.c_str(), state.presenceState.c_str());
    });

    circuit->setup();

    std::vector<String> useridsToSubscribe;
    useridsToSubscribe.push_back("94737ebb-c9a7-4b15-9a0f-4ec281d57f1f");
    
    circuit->subscribeToPresence(useridsToSubscribe, [](Circuit::ResultCode resultCode, std::vector<Circuit::PresenceState> states) {
        Serial.printf("[Main] Received subscribe presence results");
        for (Circuit::PresenceState s : states) {
            Serial.printf("[Main] Presence state of user ::= [%s] is ::= [%s]\n", s.userId.c_str(), s.presenceState.c_str());
        }
    });
}

uint64_t last = millis();
int counter = 1;

void loop() {
    circuit->loop();
    /*
    uint64_t now = millis();

    if (now - last > 10000) {
        String message = String("Test Message: ") + String(counter++);
        circuit->post("b88609d3-9330-4266-9bbb-7e1263771e39", message, "", [](Circuit::ResultCode resultCode, Circuit::TextItem item) {
            Serial.printf("[Main] Response for new text item creation ::= [%s]\n", item.toString().c_str());
        });
        last = now;
    }
     */
}