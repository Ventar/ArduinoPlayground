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

enum Mode { OFF, WHITE, ALARM, RAINBOW, PRESENCE };

// const String WIFI_NAME = "MRO Finger Weg";
// const String WIFI_PASSWORT = "7b559b98adc2";
const String WIFI_NAME = "Destiny_EXT";
const String WIFI_PASSWORT = "8882941015907883";

const String CIRCUIT_SYSTEM = "hakuna.circuitsandbox.net";
const String CIRCUIT_USER = "michael.rodenbuecher@spaces.com";
const String CIRCUIT_PWD = "sdf56JKL!";

NeoCube* c1 = new NeoCube("b6a6e636-8cab-4a70-b7a3-b58042e646d3");  // Frank Poettgen
NeoCube* c2 = new NeoCube("cd32a2eb-23d4-456d-995d-6be3c233d4ca");  // Yannick Ruppenthal
NeoCube* c3 = new NeoCube("2e1e1fe1-2c11-4502-9d06-19bb43bc9393");  // Nicole Ziegenhagen
NeoCube* c4 = new NeoCube("a6e84c2e-d0fb-4b25-8b83-2c1ea55c41bd");  // Lukas Ruge

leds::AsyncNeoPixel strip(40, D4, NEO_GRB + NEO_KHZ800);
webserver::WebServer server(80);
Circuit::CircuitClient* circuit;

std::vector<String> userIds;
std::vector<NeoCube*> neoCubes;
Mode mode = RAINBOW;

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
  if (mode == PRESENCE) {
    String colorString = "000000,000000,000000,000000,";
    for (NeoCube* cube : neoCubes) {
      for (uint8_t i = 0; i < 9; i++) {
        colorString += cube->color + ",";
      }
    }

    colorString = colorString.substring(0, colorString.length() - 1);
    Serial.println(colorString);
    strip.setMode(leds::AsyncNeoPixelMode::STATIC, colorString);
  } else if (mode == RAINBOW) {
    strip.setMode(leds::AsyncNeoPixelMode::RAINBOW, "", "96");
  } else if (mode == ALARM) {
    String colorString =
        "FF0000,0000FF,FF0000,0000FF,FF0000,0000FF,FF0000,0000FF,FF0000,0000FF,"
        "FF0000,0000FF,FF0000,0000FF,FF0000,0000FF,FF0000,0000FF,FF0000,0000FF,"
        "FF0000,0000FF,FF0000,0000FF,FF0000,0000FF,FF0000,0000FF,FF0000,0000FF,"
        "FF0000,0000FF,FF0000,0000FF,FF0000,0000FF,FF0000,0000FF,FF0000,0000FF,";
    strip.setMode(leds::AsyncNeoPixelMode::THEATER_CHASE, colorString, "96");
  } else if (mode == OFF) {
    strip.setMode(leds::AsyncNeoPixelMode::OFF);
  } else if (mode == WHITE) {
    String colorString =
        "222222,222222,222222,222222,222222,222222,222222,222222,222222,222222,"
        "222222,222222,222222,222222,222222,222222,222222,222222,222222,222222,"
        "222222,222222,222222,222222,222222,222222,222222,222222,222222,222222,"
        "222222,222222,222222,222222,222222,222222,222222,222222,222222,222222,";
    strip.setMode(leds::AsyncNeoPixelMode::STATIC, colorString);
  }
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

  server.setup();
  server.onHttpRequest("/alarm", [](ESP8266WebServer* server) {
    mode = ALARM;
    setLEDColors();
    server->send(200, "text/plain", "ok");
  });
  server.onHttpRequest("/rainbow", [](ESP8266WebServer* server) {
    mode = RAINBOW;
    setLEDColors();
    server->send(200, "text/plain", "ok");
  });

  server.onHttpRequest("/presence", [](ESP8266WebServer* server) {
    mode = PRESENCE;
    setLEDColors();
    server->send(200, "text/plain", "ok");
  });

  server.onHttpRequest("/off", [](ESP8266WebServer* server) {
    mode = OFF;
    setLEDColors();
    server->send(200, "text/plain", "ok");
  });

  server.onHttpRequest("/white", [](ESP8266WebServer* server) {
    mode = WHITE;
    setLEDColors();
    server->send(200, "text/plain", "ok");
  });

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
  server.loop();
}
