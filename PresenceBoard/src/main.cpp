#include <Arduino.h>
#include <CircuitClient.h>
#include <CommonUtilities.h>
#include <vector>

struct NeoCube {
  NeoCube(uint16_t ledStart, uint16_t ledEnd, String userId) {
    this->ledEnd = ledEnd;
    this->ledStart = ledStart;
    this->userId = userId;

  }

  uint16_t ledStart;
  uint16_t ledEnd;
  String userId;
  String color = "222222";

};

const String WIFI_NAME = "MRO Finger Weg";
const String WIFI_PASSWORT = "7b559b98adc2";

// const String WIFI_NAME = "Destiny_EXT";
// const String WIFI_PASSWORT = "8882941015907883";

const String CIRCUIT_SYSTEM = "hakuna.circuitsandbox.net";
const String CIRCUIT_USER = "michael.rodenbuecher@spaces.com";
const String CIRCUIT_PWD = "sdf56JKL!";

const NeoCube NeoCube1(0,8, "69ac9075-0eae-4c7a-a7ec-911ecc9bbb58"); // Mischa van Oijen
const NeoCube NeoCube2(9,17, "55749dad-e394-4349-b4c6-ff94c10c0f00"); // Michael Schuergers
const NeoCube NeoCube3(18,26, "c822303d-262c-4340-8f62-c94b0f757eb8"); // Alper Teke
const NeoCube NeoCube4(27,35, "0b2aa684-fba1-4b73-ab31-64902076eed1"); // Rodrigo Pastro

Circuit::CircuitClient* circuit;
std::vector<String> userIds;

/**
 * Registers a callback in the circuit client to get informed about new
 * messages.
 */
void registerPresenceStateChangedCallback() {
  circuit->onUserUserPresenceChangedEvent([](Circuit::PresenceState state) {
    Serial.printf("Presence state of user ::= [%s] is ::= [%s]", state.userId.c_str(), state.presenceState.c_str());
  });
}

void setup() {
    CommonUtilities::Utilities::setupSerial("--            Presence Board         --");
    CommonUtilities::Utilities::connectToWifi(WIFI_NAME, WIFI_PASSWORT);

    
    userIds.push_back(NeoCube1.userId); 
    userIds.push_back(NeoCube2.userId); 
    userIds.push_back(NeoCube3.userId); 
    userIds.push_back(NeoCube4.userId); 
        
    
    circuit = new Circuit::CircuitClient(CIRCUIT_SYSTEM, 443, CIRCUIT_USER, CIRCUIT_PWD);
    circuit->setup();
    circuit->subscribeToPresence(userIds, [] (Circuit::ResultCode resultCode, std::vector<Circuit::PresenceState> states) {

    });
    registerPresenceStateChangedCallback();
}

void loop() {
  circuit->loop();
}

