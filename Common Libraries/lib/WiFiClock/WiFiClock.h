#ifndef WiFiClock_h
#define WiFiClock_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>


#define DEBUG_WIFI_CLOCK

#ifdef DEBUG_WIFI_CLOCK


#define debug(x, ...) Serial.printf((String(x) + String("\n")).c_str(), __VA_ARGS__);
#define debugln(x) Serial.printf((String(x) + String("\n")).c_str());

#else
#define debugln(x)
#define debug(x, ...)
#endif

/**
 * Class to fetch the time from http://worldclockapi.com/api/json/cst/now. The call is executed once during the setup
 * method and afterwards every x hours to synchronize the time.
 * The class relies on an existing WiFi connection, i.e. you have to ensure that a WiFi connection was setup before
 * calling the setup() method of this class.
 */
class WiFiClock {
  /**
   * Event triggered when the time was updated
   */
  typedef std::function<void(String time)> TimeUpdatedEvent;

 public:

  /**
   * Sets an http proxy for the clock.
   *
   * @param String host the proxy host
   * @param uint16_t port the proxy port
   */
  void setProxy(String host, uint16_t port) {
    this->_proxyHost = host;
    this->_proxyPort = port;
  }

  /**
   * Initial setup of the class. Executes a first HTTP call to the world click api to fetch the time.
   */
  void setup() {
    _lastColonSwitchTime = millis();
    _showColon = true;
    fetchTime();
  };

  /**
   * Updates the time based on the last fetched time from the REST API and the millis() since the board was started. If
   * the update interval has elapsed a new call to the REST API is executed to sync the time.
   * The method has to be called within the loop() method of the main programm to update the time.
   */
  void loop() {
    unsigned long now = millis();

    // the interval to update has elapsed, we need to fetch a new reference time.
    if (_lastCall == 0 || now - _lastCall >= (int)3600000) {
      fetchTime();
    } else {
      // calculate the number of minutes that have passed since the last REST update.
      _minutePassed = (now - _lastCall + _secondOffset * 1000) / 1000 / 60;
    }

    if (now - _lastColonSwitchTime > 1000) {
      _lastColonSwitchTime = now;
      _showColon = !_showColon;

      if (_updateEvent) {
        _updateEvent(getTime());
      }
    }
  };

  /**
   * Builds a time String from the current time.
   */
  String getTime() {
    String time = getHours() < 10 ? "0" + String(getHours()) : String(getHours());
    time += !_blink || _showColon ? ":" : " ";
    time += getMinutes() < 10 ? "0" + String(getMinutes()) : String(getMinutes());

    return time;
  }

  /**
   * Returns the current hour of the clock in 24h format
   */
  short getHours() {
    if (_minute + _minutePassed > 59 && _hour <= 24) {
      return _hour + ((_minute + _minutePassed) / 60);
    } else if (_minute + _minutePassed > 59 && _hour > 24) {
      return (_hour + ((_minute + _minutePassed) / 60)) % 24;
    } else if (_hour >= 24) {
      return _hour % 24;
    } else {
      return _hour;
    }
  }

  /**
   * Returns the current minutes of the clock
   * @return the current minutes
   */
  short getMinutes() { return (_minute + _minutePassed) % 60; }

  /**
   * Event executed when the time has changed, either due to a blinking colon or due to a minute change.
   */
  void onUpdate(TimeUpdatedEvent event) { _updateEvent = event; }

  /**
   * If the colon of the time should blink or not
   * @param bool blink true if the colon should blink, false otherwise.
   */
  void setBlinkingColon(bool blink) { _blink = blink; }

 private:
  /**
   * Parses the response from the REST call to the API to retrieve the current values for hour and minute.
   *
   * @param String payload the HTTP body of the response
   */
  void parseHttpResponse(String payload) {
    DynamicJsonDocument doc(2048);
    DeserializationError err = deserializeJson(doc, payload);

    if (err) {
      debug("[WiFi Clock] deserializeJson() failed with code %s", err.c_str());
    } else {
      const char* dateTime = doc["datetime"];
      debug("[WiFi Clock] Received dateTime: %s\n", dateTime);
      _hour = String(dateTime).substring(11, 13).toInt();
      _minute = String(dateTime).substring(14, 16).toInt();
      _secondOffset = String(dateTime).substring(17, 19).toInt();
      debug("[WiFi Clock] Hour: %d\n", _hour);
      debug("[WiFi Clock] Minute: %d\n", _minute);
      debug("[WiFi Clock] Seconds: %d\n", _secondOffset);
    }
  }

  /**
   * Calls http://worldtimeapi.org/api/timezone/Europe/Berlin to fetch the current time
   */
  void fetchTime() {
    unsigned long now = millis();

    debugln("[WiFi Clock] Fetch time from http://worldtimeapi.org/api/timezone/Europe/Berlin");

    if (_proxyHost != "") {
      _http.connect(_proxyHost, _proxyPort);
      _http.print(String("") +
                  "GET http://worldtimeapi.org/api/timezone/Europe/Berlin HTTP/1.1\r\n"
                  "Host: worldtimeapi.org\r\n"
                  "Connection: close\r\n"
                  "Cache-Control: no-cache\r\n\r\n");
    } else {
      debugln("Fetch time without proxy");
      _http.connect("worldtimeapi.org", 80);
      _http.print(String("") +
                  "GET /api/timezone/Europe/Berlin HTTP/1.1\r\n"
                  "Host: worldtimeapi.org\r\n"
                  "Connection: close\r\n"
                  "Cache-Control: no-cache\r\n\r\n");
    }

    while (_http.connected()) {
      String line = _http.readStringUntil('\n');
      debug("[WiFi Clock] %s", line.c_str());
      if (line == "\r") {
        break;
      }
    }

    debugln("[WiFi Clock]  reply was:");
    debugln("[WiFi Clock]  ==========");
    String line;
    while (_http.available()) {
      line = _http.readStringUntil('\n');
      debug("[WiFi Clock] %s", line.c_str());
    }
    debugln("[WiFi Clock] ==========");
    debugln("[WiFi Clock] closing connection");

    parseHttpResponse(line);

    _lastCall = now;
    _minutePassed = 0;
  }

  /**
   * WifiClient used for the HTTP connection
   */
  WiFiClient _http;
  /**
   * Configured procy host.
   */
  String _proxyHost;
  /**
   * Configured proxy port
   */
  uint16_t _proxyPort;
  /**
   *Last time the clock was updated with data from the REST API
   */
  unsigned long _lastCall = 0;
  /**
   * Hour value of the last update, i.e. not the current time but the time at _lastCall
   */
  short _hour = 0;
  /**
   * Minute value of the last update, i.e. not the current time but the time at _lastCall
   */
  short _minute = 0;
  /**
   * Minutes passed since the last update (_lastCall). Used to calculate the current time.
   */
  short _minutePassed = 0;
  /**
   * Seconds offset. Since the millis() function is depending on the start time of the board we cannot use it to
   * calculate the current minutes without using this offset to find the real minute.
   */
  short _secondOffset = 0;
  /**
   * If the colon of the timestamp should blink or not.
   */
  bool _blink = false;
  /**
   * Last time the colon was either shown or not.
   */
  unsigned long _lastColonSwitchTime;
  /**
   * If the colon is currently shown or not
   */
  bool _showColon;
  /**
   * Event triggered when the time has changed.
   */
  TimeUpdatedEvent _updateEvent;
};

#endif
