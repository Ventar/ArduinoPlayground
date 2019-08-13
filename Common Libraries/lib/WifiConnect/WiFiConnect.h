#ifndef WiFiConnect_h
#define WiFiConnect_h

#include <Arduino.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include <WiFiClient.h>
#include <WiFiManager.h>

class WiFiConnect {
public:
    WiFiConnect(char const *apName, char const *apPwd) {
        _accessPointName = apName;
        _accessPointPassword = apPwd;
        wifiManager.setDebugOutput(true);
    }

    void setDebugOutput(bool debug) { wifiManager.setDebugOutput(debug); }

    void setAccessPointName(char const *apName) { _accessPointName = apName; }

    void setAccessPointPassword(char const *apPwd) { _accessPointPassword = apPwd; }

    void setAccessPointCallback(void (*callback)(WiFiManager *myWiFiManager)) { wifiManager.setAPCallback(callback); }

    void setConnectedCallback(void (*callback)(String ipAddress, String SSID)) { _connectedCallback = callback; }

    void setup() {
        Serial.println("WiFiConnect::setup -  Connect executed, try to connect to WiFi network ...");
        Serial.printf("WiFiConnect::setup Start auto connect with name ::=[%s], password ::= [%s]", _accessPointName,
                      _accessPointPassword);
        wifiManager.autoConnect(_accessPointName, _accessPointPassword);
        Serial.printf("WifiConnect::setup - Connected to IP ::= [%s], SSID ::= [%s], MAC Address ::= [%s]",
                      WiFi.localIP().toString().c_str(), WiFi.SSID().c_str(), WiFi.macAddress().c_str());
    }

    void reset() { wifiManager.resetSettings(); }

private:
    char const *_accessPointName;
    char const *_accessPointPassword;
    void (*_connectedCallback)(String ipAddress, String SSID) = NULL;
    WiFiManager wifiManager;
};

#endif
