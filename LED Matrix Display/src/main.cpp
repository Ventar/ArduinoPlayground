#include <Arduino.h>
#include <CircuitClient.h>
#include <CommonUtilities.h>
#include <ESP8266WiFi.h>
#include <LEDMatrixDisplay.h>
#include <Menu.h>
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

enum ActivePage { EVENT, CLOCK, MESSAGES };

Circuit::CircuitClient* circuit;
LEDMatrixDisplay display;
WiFiClock wifiClock;
Menu::Menu menu;

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
    newMessages++;

    // TODO: Filter own messages
    displayTimer = 6000;
    setActivePage(EVENT, false);
    display.display("New Message", PA_RIGHT, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT, true);
    lastDisplayAction = millis();
  });
}

void setup() {
  // setup the display to ensure it is black
  // ----------------------------------------------------------------------------------
  display.setup();

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
    circuit = new Circuit::CircuitClient("circuitsandbox.net", 443, "michael.rodenbuecher@arduino.com", "sdf56JKL!");
    circuit->setup();
    registerNewMessageCallback();
  });

  // setup the clock
  // ----------------------------------------------------------------------------------
  display.displayTextSync("Clock", []() {
    wifiClock.setup();
    wifiClock.setBlinkingColon(true);
  });

  // display.setText(wifiClock.getTime(), PA_CENTER, PA_RANDOM, PA_NO_EFFECT, false, true, false);

  // the clock ist updated every second (blinking colon) but the display is only
  // changed if the
  /*
  wifiClock.onUpdate([](String time) {
    if (page == CLOCK) {
      display.setText(time, PA_CENTER, PA_PRINT, PA_NO_EFFECT);
    }
  });
  */
  // Menu entries
  // ----------------------------------------------------------------------------------

  menu.addMenuItem("Clock", "Clock");
  menu.addMenuItem("Messages", "Msg.");
  menu.addMenuItem("Setup", "Setup");
  menu.addMenuItem("IP", "IP", "Setup", []() {
    display.display(WiFi.localIP().toString(), PA_RIGHT, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT, true);
    displayTimer = 10000;
    lastDisplayAction = millis();
  });

  menu.onMenuSelected([](Menu::MenuItem* item) {
    //
  });
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
    menu.next();
    display.display(menu.getActiveItem()->displayName, PA_RIGHT, PA_SCROLL_DOWN, PA_NO_EFFECT, false);

    if (menu.getActiveItem()->id == "Clock") {
      setActivePage(CLOCK, true);
    } else if (menu.getActiveItem()->id == "Messages") {
      setActivePage(MESSAGES, true);
    }

    delay(500);
    lastDisplayAction = millis();
  }

  if (d2 == 0) {
    if (menu.getActiveItem()->children.empty()) {
      menu.execute();
    } else {
      menu.child();
      display.display(menu.getActiveItem()->displayName, PA_RIGHT, PA_SCROLL_DOWN, PA_NO_EFFECT, false);
      lastDisplayAction = millis();
    }
    delay(500);
  }

  if (d4 == 0) {
  }

  if (page == CLOCK && millis() - lastDisplayAction > 2000) {
    display.display(wifiClock.getTime(), PA_CENTER, PA_PRINT, PA_NO_EFFECT, false);
  } else if (page == MESSAGES && millis() - lastDisplayAction > 2000) {
    display.display(String("- ") + newMessages + " -", PA_CENTER, PA_PRINT, PA_NO_EFFECT, false);
  } else if (millis() - lastDisplayAction > 10000) {
    if (previousActivePage == CLOCK) {
      setActivePage(CLOCK, true);
    } else if (previousActivePage == MESSAGES) {
      setActivePage(MESSAGES, true);
    }
    displayTimer = 10000;
  }

  /**

    if (page == CLOCK) {
      display.setText(wifiClock.getTime(), PA_CENTER, PA_PRINT, PA_NO_EFFECT);
    }

    if (d1 == 0) {
      page = CLOCK;
      display.setText(wifiClock.getTime(), PA_CENTER, PA_PRINT, PA_NO_EFFECT);
      delay(500);
    }

    if (d2 == 0) {
      displayNewMessages();
      delay(500);
    }

    if (d3 == 0) {
      if (page == NEW_MESSAGES) {
        shortMessageCounter = !shortMessageCounter;
        displayNewMessages();
        delay(500);
      }
    }
  */
  /*
  if (millis() - lastPrint > 1000) {
      Serial.printf("d1: %d / d2: %d / d3: %d / d4: %d\n", d1, d2, d3, d4);
      lastPrint = millis();
  }
  */
}
