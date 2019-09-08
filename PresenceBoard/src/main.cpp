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

enum Mode { OFF, WHITE, ALARM, RAINBOW, CIRCUIT, CALL };

// const String WIFI_NAME = "MRO Finger Weg";
// const String WIFI_PASSWORT = "7b559b98adc2";
const String WIFI_NAME = "Destiny_EXT";
const String WIFI_PASSWORT = "8882941015907883";

// const String CIRCUIT_SYSTEM = "hakuna.circuitsandbox.net";
// const String CIRCUIT_USER = "michael.rodenbuecher@spaces.com";
// const String CIRCUIT_PWD = "sdf56JKL!";
// NeoCube* c1 = new NeoCube("b6a6e636-8cab-4a70-b7a3-b58042e646d3");  // Frank Poettgen
// NeoCube* c2 = new NeoCube("cd32a2eb-23d4-456d-995d-6be3c233d4ca");  // Yannick Ruppenthal
// NeoCube* c3 = new NeoCube("2e1e1fe1-2c11-4502-9d06-19bb43bc9393");  // Nicole Ziegenhagen
// NeoCube* c4 = new NeoCube("a6e84c2e-d0fb-4b25-8b83-2c1ea55c41bd");  // Lukas Ruge

const String CIRCUIT_SYSTEM = "circuitsandbox.net";
const String CIRCUIT_USER = "michael.rodenbuecher@arduino.com";
const String CIRCUIT_PWD = "sdf56JKL!";
const String CIRCUIT_USER_ID = "2f9609c6-de11-44df-957f-6ba3383a0025";
const String CIRCUIT_CONV_ID = "37ac0211-5fae-41c4-b464-d0802a296b9e";

NeoCube* c1 = new NeoCube("81fad5a6-212f-4dca-8b40-78742a1aca79");  // Frank Poettgen
NeoCube* c2 = new NeoCube("f0005d36-4484-4323-97b6-9858c92e5a5d");  // Yannick Ruppenthal
NeoCube* c3 = new NeoCube("392d2ec8-44cf-42f9-b88b-3ed9c332493d");  // Nicole Ziegenhagen
NeoCube* c4 = new NeoCube("3eb69cff-5f45-428b-9a9a-37f1b584b756");  // Lukas Ruge

leds::AsyncNeoPixel strip(45, D1, NEO_GRB + NEO_KHZ800);
webserver::WebServer server(80);
Circuit::CircuitClient* circuit;

std::vector<String> userIds;
std::vector<NeoCube*> neoCubes;
Mode mode = OFF;
Mode previousMode = OFF;
uint64_t eventDetected;

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
  if (mode == CIRCUIT) {
    String colorString = "000000,000000,000000,000000,000000,000000,000000,000000,000000,";
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
        "FF0000,0000FF,FF0000,0000FF,FF0000,0000FF,FF0000,0000FF,FF0000,0000FF,"
        "FF0000,0000FF,FF0000,0000FF,FF0000,0000FF,FF0000,0000FF,FF0000,0000FF,";
    strip.setMode(leds::AsyncNeoPixelMode::THEATER_CHASE, colorString, "90");
  } else if (mode == CALL) {
    String colorString =
        "0CA71F,80F78F,0CA71F,80F78F,0CA71F,80F78F,0CA71F,80F78F,0CA71F,80F78F,"
        "0CA71F,80F78F,0CA71F,80F78F,0CA71F,80F78F,0CA71F,80F78F,0CA71F,80F78F,"
        "0CA71F,80F78F,0CA71F,80F78F,0CA71F,80F78F,0CA71F,80F78F,0CA71F,80F78F,"
        "0CA71F,80F78F,0CA71F,80F78F,0CA71F,80F78F,0CA71F,80F78F,0CA71F,80F78F,"
        "0CA71F,80F78F,0CA71F,80F78F,0CA71F,80F78F,0CA71F,80F78F,0CA71F,80F78F,";
    strip.setMode(leds::AsyncNeoPixelMode::THEATER_CHASE, colorString, "90");
  } else if (mode == OFF) {
    strip.setMode(leds::AsyncNeoPixelMode::OFF);
  } else if (mode == WHITE) {
    String colorString =
        "222222,222222,222222,222222,222222,222222,222222,222222,222222,222222,"
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

/**
 * Registers a callback in the circuit client to get informed about new
 * calls.
 */
void registerIncomingCallCallback() {
  circuit->onRTCSessionSessionStartedEvent([](Circuit::Call call) {
    if (mode == CIRCUIT) {
      eventDetected = millis();
      previousMode = mode;
      mode = CALL;
      setLEDColors();
    }
  });
}

void setup() {
  strip.setup();
  strip.setMode(leds::AsyncNeoPixelMode::STATIC,
                "ff0000,ff0000,ff0000,ff0000,ff0000,ff0000,ff0000,ff0000,ff0000,000000");

  server.setup();
  server.onHttpRequest("/alarm", [](ESP8266WebServer* server) {
    if (mode != ALARM) {
      previousMode = mode;
      mode = ALARM;
    }
    eventDetected = millis();
    setLEDColors();
    server->send(200, "text/plain", "ok");
  });
  server.onHttpRequest("/rainbow", [](ESP8266WebServer* server) {
    mode = RAINBOW;
    previousMode = RAINBOW;
    setLEDColors();
    server->send(200, "text/plain", "ok");
  });

  server.onHttpRequest("/circuit", [](ESP8266WebServer* server) {
    mode = CIRCUIT;
    previousMode = CIRCUIT;
    setLEDColors();
    server->send(200, "text/plain", "ok");
  });

  server.onHttpRequest("/off", [](ESP8266WebServer* server) {
    mode = OFF;
    previousMode = OFF;
    setLEDColors();
    server->send(200, "text/plain", "ok");
  });

  server.onHttpRequest("/white", [](ESP8266WebServer* server) {
    mode = WHITE;
    previousMode = WHITE;
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
  });

  registerPresenceStateChangedCallback();
  registerIncomingCallCallback();

  strip.setMode(leds::AsyncNeoPixelMode::STATIC,
                "00F700,00F700,00F700,00F700,00F700,00F700,00F700,00F700,00F700,000000");
  eventDetected = millis();
}

void loop() {
  circuit->loop();
  strip.loop();
  server.loop();

  uint64_t now = millis();

  if (now - eventDetected > 10000 && (mode == CALL || mode == ALARM)) {
    mode = previousMode;
    setLEDColors();
  }
}
