#include <Arduino.h>
#include <CircuitClient.h>
#include <CommonUtilities.h>
#include <ESP8266WiFi.h>
#include <LEDMatrixDisplay.h>
#include <Menu.h>
#include <PresenceState.h>
#include <TextItem.h>
#include <WebServer.h>
#include <WiFiClock.h>

// ---------------------------------------------------------------------------------------------------------
// Config
// ---------------------------------------------------------------------------------------------------------

// const String WIFI_NAME = "MRO Finger Weg";
// const String WIFI_PASSWORT = "7b559b98adc2";

const String WIFI_NAME = "Destiny_EXT";
const String WIFI_PASSWORT = "8882941015907883";

const String CIRCUIT_SYSTEM = "hakuna.circuitsandbox.net";
const String CIRCUIT_USER = "michael.rodenbuecher@spaces.com";
const String CIRCUIT_PWD = "sdf56JKL!";
const String CIRCUIT_USER_ID = "2f9609c6-de11-44df-957f-6ba3383a0025";
const String CIRCUIT_CONV_ID = "37ac0211-5fae-41c4-b464-d0802a296b9e";
const String SPHERE_IP = "192.168.2.106";
const String BOARD_IP = "192.168.2.113";

// ---------------------------------------------------------------------------------------------------------
// Components
// ---------------------------------------------------------------------------------------------------------
Circuit::CircuitClient* circuit;
LEDMatrixDisplay display;
WiFiClock wifiClock;
Menu::Menu menu;
webserver::WebServer server(80);

// ---------------------------------------------------------------------------------------------------------
// Data
// ---------------------------------------------------------------------------------------------------------

enum ActivePage { EVENT, CLOCK, MESSAGES };

// Time when to content of the display changed last time.
uint64_t lastDisplayAction = millis();

// The number of messages whichw where received during the lifetime of the
// appliation
int newMessages = 0;

// The page which is currently active
ActivePage page = CLOCK;

// the previous active page
ActivePage previousActivePage = CLOCK;

/**
 * Time the display remains in the current state until it oes back to the selected page.
 */
uint16_t displayTimer = 10000;

/**
 * Sets the values for the active page and stores the current value in the previous field.
 */
void setActivePage(ActivePage newPage, bool overridePrevious) {
  if (overridePrevious) {
    previousActivePage = newPage;
  } else {
    previousActivePage = page;
  }
  page = newPage;
}

/**
 * Registers a callback in the circuit client to get informed about new
 * messages.
 */
void registerNewMessageCallback() {
  circuit->onConversationAddItemEvent([](Circuit::TextItem item) {
    if (item.creatorId != CIRCUIT_USER_ID) {
      newMessages++;

      // TODO: Filter own messages
      displayTimer = 6000;
      setActivePage(EVENT, false);
      display.display("New Message", PA_RIGHT, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT, true);
      lastDisplayAction = millis();
    }
  });
}

/**
 * Registers a callback in the circuit client to get informed about new
 * calls.
 */
void registerIncomingCallCallback() {
  circuit->onRTCSessionSessionStartedEvent([](Circuit::Call call) {
    displayTimer = 8000;
    setActivePage(EVENT, false);
    display.display("Call", PA_CENTER, PA_OPENING, PA_CLOSING, true);
    lastDisplayAction = millis();
  });
}

void setup() {
  // setup the display to ensure it is black
  // ----------------------------------------------------------------------------------
  display.setup();
  server.setup();
  server.onHttpRequest("/alarm", [](ESP8266WebServer* server) {
    circuit->post(CIRCUIT_CONV_ID, wifiClock.getISO8601() + " alarm detected !!!", "Alarm",
                  [](Circuit::ResultCode resultCode, Circuit::TextItem item) {
                    Serial.printf("[Main] Response for new text item creation ::= [%s]\n", item.toString().c_str());
                  });

    display.display("Alarm detected post to Circuit", PA_RIGHT, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT, true);
    displayTimer = 12000;
    lastDisplayAction = millis();
    server->send(200, "text/plain", "ok");
  });

  // Buttons ( will be quite fast)
  // ----------------------------------------------------------------------------------
  pinMode(D4, INPUT);
  pinMode(D3, INPUT);
  pinMode(D2, INPUT);
  pinMode(D1, INPUT);

  // General setup for serial connection and Wifi
  // ----------------------------------------------------------------------------------
  display.displayTextSync("WiFi", []() {
    CommonUtilities::Utilities::setupSerial("--          LED Matrix Display       --");
    CommonUtilities::Utilities::connectToWifi(WIFI_NAME, WIFI_PASSWORT);
  });

  // Setup the Circuit connection
  // ----------------------------------------------------------------------------------
  display.displayTextSync("Circuit", []() {
    circuit = new Circuit::CircuitClient(CIRCUIT_SYSTEM, 443, CIRCUIT_USER, CIRCUIT_PWD);
    circuit->setup();
    registerNewMessageCallback();
    registerIncomingCallCallback();
  });

  // setup the clock
  // ----------------------------------------------------------------------------------
  display.displayTextSync("Clock", []() {
    wifiClock.setup();
    wifiClock.setBlinkingColon(true);
  });

  // Menu entries
  // ----------------------------------------------------------------------------------

  menu.addMenuItem("Clock", "Clock", []() { setActivePage(CLOCK, true); });
  menu.addMenuItem("Messages", "Msg.", []() { setActivePage(MESSAGES, true); });
  menu.addMenuItem("Setup", "Setup");
  menu.addMenuItem("IP", "IP", "Setup", []() {
    display.display(WiFi.localIP().toString(), PA_RIGHT, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT, true);
    displayTimer = 10000;
    lastDisplayAction = millis();
  });
  menu.addMenuItem("Sphere", "Sphere");
  menu.addMenuItem("SphereEnable", "On", "Sphere", []() {
    HTTPClient http;
    http.begin(String("http://") + SPHERE_IP + "/radar/enable");
    int code = http.GET();
    Serial.printf("received HTTP response code %d for request to enable radar", code);
    http.end();
    display.display("Movement detection enabled", PA_RIGHT, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT, true);
    displayTimer = 10000;
    lastDisplayAction = millis();
  });
  menu.addMenuItem("SphereEnable", "Off", "Sphere", []() {
    HTTPClient http;
    http.begin(String("http://") + SPHERE_IP + "/radar/disable");
    int code = http.GET();
    Serial.printf("received HTTP response code %d for request to enable radar", code);
    http.end();
    display.display("Movement detection disabled", PA_RIGHT, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT, true);
    displayTimer = 10000;
    lastDisplayAction = millis();
  });
  menu.addMenuItem("Board", "Board");
  menu.addMenuItem("BoardOff", "Off", "Board", []() {
    HTTPClient http;
    http.begin(String("http://") + BOARD_IP + "/off");
    int code = http.GET();
    Serial.printf("received HTTP response code %d for request to change presence board", code);
    http.end();
    display.display("Presence Board set to mode OFF", PA_RIGHT, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT, true);
    displayTimer = 12000;
    lastDisplayAction = millis();
  });
  menu.addMenuItem("BoardAlarm", "Alarm", "Board", []() {
    HTTPClient http;
    http.begin(String("http://") + BOARD_IP + "/alarm");
    int code = http.GET();
    Serial.printf("received HTTP response code %d for request to change presence board", code);
    http.end();
    display.display("Presence Board set to mode ALARM", PA_RIGHT, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT, true);
    displayTimer = 12000;
    lastDisplayAction = millis();
  });
  menu.addMenuItem("BoardLight", "Light", "Board", []() {
    HTTPClient http;
    http.begin(String("http://") + BOARD_IP + "/white");
    int code = http.GET();
    Serial.printf("received HTTP response code %d for request to change presence board", code);
    http.end();
    display.display("Presence Board set to mode LIGHT", PA_RIGHT, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT, true);
    displayTimer = 12000;
    lastDisplayAction = millis();
  });
  menu.addMenuItem("BoardPresence", "Pres.", "Board", []() {
    HTTPClient http;
    http.begin(String("http://") + BOARD_IP + "/presence");
    int code = http.GET();
    Serial.printf("received HTTP response code %d for request to change presence board", code);
    http.end();
    display.display("Presence Board set to mode PRESENCE", PA_RIGHT, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT, true);
    displayTimer = 12000;
    lastDisplayAction = millis();
  });
  menu.addMenuItem("BoardRainbow", "Rain.", "Board", []() {
    HTTPClient http;
    http.begin(String("http://") + BOARD_IP + "/rainbow");
    int code = http.GET();
    Serial.printf("received HTTP response code %d for request to change presence board", code);
    http.end();
    display.display("Presence Board set to mode RAINBOW", PA_RIGHT, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT, true);
    displayTimer = 12000;
    lastDisplayAction = millis();
  });

  menu.onMenuSelected([](Menu::MenuItem* item) {
    //
  });
}

void loop() {
  uint64_t now = millis();
  wifiClock.loop(now);
  display.loop();
  circuit->loop();
  server.loop();

  int d1 = digitalRead(D1);
  int d2 = digitalRead(D2);
  int d3 = digitalRead(D3);
  int d4 = digitalRead(D4);

  if (d1 == 0) {
    menu.next();
    display.display(menu.getActiveItem()->displayName, PA_RIGHT, PA_SCROLL_DOWN, PA_NO_EFFECT, false);
    delay(500);
    lastDisplayAction = now;
  }

  if (d2 == 0) {
    if (menu.getActiveItem()->children.empty()) {
      menu.execute();
    } else {
      menu.child();
      display.display(menu.getActiveItem()->displayName, PA_RIGHT, PA_SCROLL_DOWN, PA_NO_EFFECT, false);
      lastDisplayAction = now;
    }
    delay(500);
  }

  if (d3 == 0) {
    menu.back();
    display.display(menu.getActiveItem()->displayName, PA_RIGHT, PA_SCROLL_DOWN, PA_NO_EFFECT, false);
    lastDisplayAction = now;
    delay(500);
  }

  if (page == CLOCK && millis() - lastDisplayAction > 5000) {
    display.display(wifiClock.getTime(), PA_CENTER, PA_PRINT, PA_NO_EFFECT, false);
  } else if (page == MESSAGES && millis() - lastDisplayAction > 5000) {
    display.display(String("- ") + newMessages + " -", PA_CENTER, PA_PRINT, PA_NO_EFFECT, false);
  } else if (millis() - lastDisplayAction > 10000) {
    if (previousActivePage == CLOCK) {
      setActivePage(CLOCK, true);
    } else if (previousActivePage == MESSAGES) {
      setActivePage(MESSAGES, true);
    }
    displayTimer = 10000;
  }
}
