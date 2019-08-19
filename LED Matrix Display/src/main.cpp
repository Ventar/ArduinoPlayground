#include <Arduino.h>
#include <CircuitClient.h>
#include <CommonUtilities.h>
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

Circuit::CircuitClient* circuit;
LEDMatrixDisplay display;
WiFiClock wifiClock;
Menu::Menu menu;

enum ActivePage { CLOCK, NEW_MESSAGES, INCOMING_CALL, DISPLAY_EVENT };

// Boolean flag that indicates if messages are shown as plain number or
// scrolling text
bool shortMessageCounter = false;

// The number of messages whichw where received during the lifetime of the
// appliation
int newMessages = 0;

// The page which is currently active
ActivePage page = CLOCK;

// The page thatw as set before an incoming event. Used to display this page
// after the event processing
ActivePage preEventPage;

/**
 * Registers a callback in the circuit client to get informed about new
 * messages.
 */
void registerNewMessageCallback() {
  circuit->onConversationAddItemEvent([](Circuit::TextItem item) {
    newMessages++;
    preEventPage = page;
    page = DISPLAY_EVENT;

    switch (preEventPage) {
      case CLOCK:
        display.setText(wifiClock.getTime(), PA_CENTER, PA_SCROLL_DOWN, PA_NO_EFFECT, false, true, false);
        break;
    }
    /*
        display.setTextWithCallback(String("New Message"), PA_RIGHT, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT, []() {
          display.setText(wifiClock.getTime(), PA_CENTER, PA_SCROLL_DOWN, PA_NO_EFFECT, false, false);
          page = preEventPage;
          
        });*/
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
  menu.addMenuItem("Messages", "Messages");
  menu.addMenuItem("Setup", "Setup");
  menu.addMenuItem("IP", "IP", "Setup");

  menu.onMenuSelected([](Menu::MenuItem* item) {
    //
  });
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
    menu.next();
    display.displayInitAndShow(menu.getActiveItem()->displayName, PA_RIGHT, PA_SCROLL_DOWN, PA_SCROLL_RIGHT,
                               PA_SCROLL_RIGHT, true);
    delay(500);
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
