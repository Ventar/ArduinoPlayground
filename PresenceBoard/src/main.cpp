#include <Arduino.h>
#include <AsyncNeoPixel.h>
#include <CircuitClient.h>
#include <CommonUtilities.h>
#include <WebServer.h>
#include <vector>

struct NeoCube {
  NeoCube(String userId) { this->userId = userId; }

  String userId;
  String color = "222222";
};

// const String WIFI_NAME = "MRO Finger Weg";
// const String WIFI_PASSWORT = "7b559b98adc2";

const String WIFI_NAME = "Destiny_EXT";
const String WIFI_PASSWORT = "8882941015907883";

const String CIRCUIT_SYSTEM = "hakuna.circuitsandbox.net";
const String CIRCUIT_USER = "michael.rodenbuecher@spaces.com";
const String CIRCUIT_PWD = "sdf56JKL!";

NeoCube* c1 = new NeoCube("69ac9075-0eae-4c7a-a7ec-911ecc9bbb58");  // Mischa van Oijen
NeoCube* c2 = new NeoCube("55749dad-e394-4349-b4c6-ff94c10c0f00");  // Michael Schuergers
NeoCube* c3 = new NeoCube("c822303d-262c-4340-8f62-c94b0f757eb8");  // Alper Teke
NeoCube* c4 = new NeoCube("0b2aa684-fba1-4b73-ab31-64902076eed1");  // Rodrigo Pastro

leds::AsyncNeoPixel strip(40, D4, NEO_GRB + NEO_KHZ800);
// webserver::WebServer server(80);
Circuit::CircuitClient* circuit;

std::vector<String> userIds;
std::vector<NeoCube*> neoCubes;

/**
 * Set the color of a cube based on the presence state.
 */
void updateNeoCube(NeoCube* cube, Circuit::PresenceState state) {
  if (state.presenceState == "DND") {
    Serial.printf("Change color of user ::= [%s] to ::= [FF0000]\n", state.userId.c_str());
    cube->color = "ff0000";
  } else if (state.presenceState == "AVAILABLE") {
    Serial.printf("Change color of user ::= [%s] to ::= [00FF00]\n", state.userId.c_str());
    cube->color = "00ff00";
  } else if (state.presenceState == "BUSY") {
    Serial.printf("Change color of user ::= [%s] to ::= [0000FF]\n", state.userId.c_str());
    cube->color = "0000ff";
  } else if (state.presenceState == "AWAY") {
    Serial.printf("Change color of user ::= [%s] to ::= [0000FF]\n", state.userId.c_str());
    cube->color = "FFFF00";
  } else {
    Serial.printf("Change color of user ::= [%s] to ::= [000000]\n", state.userId.c_str());
    cube->color = "000000";
  }
}

/**
 * Update the NeoPixels.
 */
void setLEDColors() {
  String colorString = "000000,000000,000000,000000,";
  for (NeoCube* cube : neoCubes) {
    for (uint8_t i = 0; i < 9; i++) {
      colorString += cube->color + ",";
    }
  }

  colorString = colorString.substring(0, colorString.length() - 1);
  Serial.println(colorString);
  strip.setMode(leds::AsyncNeoPixelMode::STATIC, colorString);
}

/**
 * Registers a callback in the circuit client to get informed about new
 * messages.
 */
void registerPresenceStateChangedCallback() {
  circuit->onUserUserPresenceChangedEvent([](Circuit::PresenceState state) {
    Serial.printf("Presence state of user ::= [%s] is ::= [%s]", state.userId.c_str(), state.presenceState.c_str());
    for (NeoCube* cube : neoCubes) {
      if (state.userId == cube->userId) {
        updateNeoCube(cube, state);
      }
    }
    setLEDColors();
  });
}

void setup() {
  strip.setup();
  strip.setMode(leds::AsyncNeoPixelMode::STATIC, "000000,000000,000000,000000,22FF22");

  CommonUtilities::Utilities::setupSerial("--            Presence Board         --");
  CommonUtilities::Utilities::connectToWifi(WIFI_NAME, WIFI_PASSWORT);

  userIds.push_back(c1->userId);
  userIds.push_back(c2->userId);
  userIds.push_back(c3->userId);
  userIds.push_back(c4->userId);

  neoCubes.push_back(c1);
  neoCubes.push_back(c2);
  neoCubes.push_back(c3);
  neoCubes.push_back(c4);

  circuit = new Circuit::CircuitClient(CIRCUIT_SYSTEM, 443, CIRCUIT_USER, CIRCUIT_PWD);
  circuit->setup();
  circuit->subscribeToPresence(userIds, [](Circuit::ResultCode resultCode, std::vector<Circuit::PresenceState> states) {
    for (Circuit::PresenceState state : states) {
      Serial.printf("Presence state of user ::= [%s] is ::= [%s]\n", state.userId.c_str(), state.presenceState.c_str());
      for (NeoCube* cube : neoCubes) {
        if (state.userId == cube->userId) {
          updateNeoCube(cube, state);
        }
      }
    }
    setLEDColors();
  });

  registerPresenceStateChangedCallback();
}

void loop() {
  circuit->loop();
  strip.loop();
  //  server.loop();
}
