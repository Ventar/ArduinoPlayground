#include "CircuitClient.h"
#include <ArduinoJson.h>
#include "CircuitUtils.h"

#define DEBUG_CIRCUIT

#ifdef DEBUG_CIRCUIT

#define debug(x, ...) Serial.printf((String(x) + String("\n")).c_str(), __VA_ARGS__);
#define debugln(x) Serial.printf((String(x) + String("\n")).c_str());

#else
#define debugln(x)
#define debug(x, ...)
#endif

namespace Circuit {

CircuitClient::CircuitClient(String host, uint16_t port, String username, String password) {
    debug("[Circuit] created client for username ::= [%s], host ::= [%s], port ::= [%d]", username.c_str(),
          host.c_str(), port);

    this->_password = password;
    this->_username = username;
    this->_host = host;
    this->_port = port;
}

// ---------------------------------------------------------------------------------------------------------------
// WEBSOCKET HANDLING
// ---------------------------------------------------------------------------------------------------------------

void CircuitClient::handleWSResponse(DynamicJsonDocument& message) {
    debugln("[Circuit] handle message of type RESPONSE");
    debug("[Circuit] open responses ::= [%d]", _openResponses.size());

    uint64_t requestId = message["response"]["requestId"];

    std::map<uint64_t, OpenResponse>::iterator it = _openResponses.find(requestId);

    if (it != _openResponses.end()) {
        debug("[Circuit] found open response with requestId ::= [%d]", it->first);
        switch (it->second.type) {
        case OpenResponse::CREATE_TEXT_ITEM:
            it->second.conversationAddTextItemCB(ResultCode(ResultCode::OK),
                                                 TextItem::fromConversationAddTextItemResponse(message));
            break;
        case OpenResponse::SUBSCRIBE_PRESENCE:
            it->second.userSubscribePresenceCB(ResultCode(ResultCode::OK),
                                               PresenceState::fromUserSubscribePresenceResponse(message));
            break;
        }
        _openResponses.erase(it);
    }

    // debug("[Circuit] number of open responses ::= [%s]\n", );
}

void CircuitClient::handleWSEvent(DynamicJsonDocument& message) {
    debugln("[Circuit] handle message of type EVENT");
    // the Circuit protocol is based on definition in Google protostuff which are serialized to JSON. As aresult we have
    // messages (objects) which aggregate other messages. Every base message send by Circuit (called WSMessage) can
    // contain all available messages of the protocol. Which concrete fields are filled depends on various type
    // parameters that define the hierachical structure of the message.
    // However the idea of this libary is not to explain the proprietary Circuit protocol but to build awesome stuff
    // with Arduino and a 3D printer, so I will offer some basic functionality here. Extensions are possible later.
    // In general we could implement a more generic solution in this place here, but I prefer to keep it easy. In
    // addition my C++ skills are quite rusty so that I will better understand the code latter :D.

    if (message["event"]["type"] == "CONVERSATION" &&  // we have a conversation event here
        message["event"]["conversation"]["type"] ==
            "ADD_ITEM" &&  // of type ADD_ITEM which is fired whenever a new item was created
        message["event"]["conversation"]["addItem"]["item"]["type"] ==
            "TEXT" &&                        // and finally this item is of type TEXT (and not SYSTEM for example)
        _conversationAddItemEventCB != NULL  // if no callback was registered we can ignore the event
    ) {
        // at this point we now that we receive a text item created event and can call the callback if it is configured
        _conversationAddItemEventCB(TextItem::fromConversationAddItemEvent(message));
    } else if (message["event"]["type"] == "RTC_SESSION" &&
               message["event"]["rtcSession"]["type"] == "SESSION_STARTED" &&
               _rtcSessionSessionStartedEventCB != NULL) {
        _rtcSessionSessionStartedEventCB(Call::fromRTCSessionStartedEvent(message));
    } else if (message["event"]["type"] == "USER" && message["event"]["user"]["type"] == "USER_PRESENCE_CHANGE" &&
               _userUserPresenceChangedEventCB != NULL) {
        _userUserPresenceChangedEventCB(PresenceState::fromUserUserPresenceChangeEvent(message));
    }
}

void CircuitClient::onWebSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
    case WStype_DISCONNECTED:
        debugln("[Circuit] WebSocket disconnected");
        break;
    case WStype_CONNECTED:
        debug("[Circuit] WebSocket connected to ::= [%s]", _host.c_str());
        break;
    case WStype_TEXT: {
        debugln("------------------------------------------------------------------");
        debug("[Circuit] Received message: %s", payload);

        // at this point we use the amazing JSON libary to parse the incoming message which is 90% of the complex part
        // for the handling of Circuit messages. The documents can have one of three major types, REQUEST, RESPONE or
        // EVENT. The Circui backend would never send a request so that we only need to handle EVENT and RESPONSE here.
        // Now the event can be triggered at any time due to actions in the system the RESPONSE on the other hand is
        DynamicJsonDocument doc(length);
        DeserializationError err = deserializeJson(doc, payload);

        if (err) {
            debug("[Circuit] deserializeJson() failed with code ::= [%s]", err.c_str());
        } else {
            const char* msgType = doc["msgType"];

            if (String("EVENT").equals(msgType)) {
                handleWSEvent(doc);
            } else {
                handleWSResponse(doc);
            }
        }
    }

    // send message to server
    // webSocket.sendTXT("message here");
    break;
    case WStype_BIN:
        Serial.printf("[WSc] get binary length: %u\n", length);
        hexdump(payload, length);

        // send data to server
        // webSocket.sendBIN(payload, length);
        break;
    case WStype_ERROR:
        debug("[Circuit] ERROR:: WebSocket connetion reported an error: %s \n", payload) break;
    case WStype_PING:
        debugln("[Circuit] Received PING");
        break;
    case WStype_PONG:
        debugln("[Circuit] Received PONG");
    default:
        debugln("[Circuit] Websocket event was not handled");
    }
}

// ---------------------------------------------------------------------------------------------------------------
// CONNECTION SETUP AND MAINTENANCE
// ---------------------------------------------------------------------------------------------------------------

void CircuitClient::setup() {
    debugln("[Circuit] try to create access server cookie...");

    // The logon process to Circuit is a two step one. First we need to generate a cookie via an HTTP call and
    // afterwards this cookie has to be used to authenticate during the websocket upgrade.
    createAccessServerCookie();

    if (_wsCookie == NULL || _wsCookie == "")  {
        debugln("[Circuit] could not retrieve cookie...");    
        return;
    }

    debugln("[Circuit] try to open websocket connection...");
        
    // starts the connection process. At this point only the configuration is done, i.e. the name begin is a little bit
    // missleadeing. The connection itself is established during the first call to the loop() method of this class.
    _webSocket.beginSSL(_host, _port, "/api");

    // no good idea as for the access server connection but it will speed up the demo. In a real product we should honor
    // the certificates of the server.
    _webSocket.setInsecure();
    //_webSocket.enableHeartbeat(5000,2000,0);
    //_webSocket.disableHeartbeat();

    // the access server expects a special user agent header to work
    _webSocket.setUserAgent("User-Agent: Unify ;; APPLICATION  ;; ARDUINO_CLIENT ;; 1.0.0 ;; ;; ;;");

    // this is the place where we set the cookie which was generated in the createAccessServerCookie() methods
    _webSocket.setExtraHeaders((String("Cookie: ") + _wsCookie).c_str());

    // callback that is triggered when we receive a message from the Circuit system after the connection was
    // established.
    _webSocket.onEvent(
        [this](WStype_t type, uint8_t* payload, size_t length) { this->onWebSocketEvent(type, payload, length); });

    // the connection is not established yet but will be in the loop method of the websocket. To make handling
    // of the lib easier we execute this here once and wait for the websocket to be connect. That will allow the
    // user to implement some initialization logic in the setup method.

    while (!_webSocket.isConnected()) {
        _webSocket.loop();
    }
}

void CircuitClient::loop() { _webSocket.loop(); }

void CircuitClient::setProxy(String proxyHost, uint16_t proxyPort) {
    _proxyHost = proxyHost;
    _proxyPort = proxyPort;
    _webSocket.setProxy(_proxyHost, _proxyPort);
}

void CircuitClient::createAccessServerCookie() {
    // we only need the HTTP client once during startup to generate the cookie. The cookie itself should be valid for 90
    // day. This means that we have to restart the device at least every 90 days which is fine. A renewal could be
    // implemented but would make the demo code to complex.
    WiFiClientSecure httpsClient;
    
    // no good idea, but it is a prototype to demonstrat IoT stuff with Circuit and not a final product.
    httpsClient.setInsecure();

    // this is the payload for the post request we send to the Circuit access server to generate the cookie.
    String payload = String("persistent=false&username=") + urlencode(_username) + "&password=" + urlencode(_password);

    // we have to create the HTTP request manually to handle possible proxy configuration within company networks. While
    // this does not happen at home, I am sometimes working from the office in the evening and this wouldn't work
    // without proxy configuration


    if (_proxyHost != "") {
        debug("[Circuit] use proxy configuration host ::= [%s], port ::= [%d] \n", _proxyHost.c_str(), _proxyPort);

    String send = String("") + "POST https://" + _host + ":" + _port +
                          "/sdklogin HTTP/1.1\r\n"
                          "Host: " +
                          _host +
                          "\r\n"
                          "User-Agent: Unify ;; APPLICATION  ;; JAVA_TESTLIB ;; "
                          "2.9.57 ;; ;; ;;\r\n"
                          "Content-Type: application/x-www-form-urlencoded\r\n"
                          "Connection: close\r\n"
                          "Content-Length: " +
                          payload.length() +
                          "\r\n"
                          "Cache-Control: no-cache\r\n\r\n" +
                          payload;
        debug("[Circuit] send data: %s", send.c_str());

        httpsClient.connect(_proxyHost, _proxyPort);
        httpsClient.print(send);
    } else {
        debugln("[Circuit] no proxy configuration set");

        httpsClient.connect(_host, _port);
        httpsClient.print(String("") +
                          "POST /sdklogin HTTP/1.1\r\n"
                          "Host: " +
                          _host +
                          "\r\n"
                          "User-Agent: Unify ;; APPLICATION  ;; JAVA_TESTLIB ;; "
                          "2.9.57 ;; ;; ;;\r\n"
                          "Content-Type: application/x-www-form-urlencoded\r\n"
                          "Connection: close\r\n"
                          "Content-Length: " +
                          payload.length() +
                          "\r\n"
                          "Cache-Control: no-cache\r\n\r\n" +
                          payload);
    }

    // as soon as he connection is established we receive the HTTP response. This is the plain response, i.e. no special
    // methods are available to distinguish between the header, the domain, and so on. For the use case here
    // that is fine because we now the exact result that is returned by the access server and can extract the data we
    // need to proceed with the logon.
    while (httpsClient.connected()) {
        String line = httpsClient.readStringUntil('\n');
        debug("[Circuit] %s\n", line.c_str());

        // this is the cookie generated by the Circuit access server that is needed to authenticate the WS connection in
        // the next step.
        if (line.startsWith("set-cookie")) {
            _wsCookie = line.substring(12, line.indexOf(";"));
        }

        if (line == "\r") {
            break;
        }
    }

#ifdef DEBUG_CIRCUIT

    // just the content of the response, there will not be anything in here, but it maybe helpful for later
    // modifications or as a reference fr other HTTP requests in the project

    debugln("[Circuit] reply was:");
    debugln("[Circuit] ==========");
    String line;
    while (httpsClient.available()) {
        line = httpsClient.readStringUntil('\n');  // Read Line by Line
        debugln("[Circuit] " + line);              // Print response
    }
    debugln("[Circuit] ==========");
    debugln("[Circuit] closing connection");

    debug("[Circuit] Received cookie '%s'", _wsCookie.c_str());
#endif
}

// ---------------------------------------------------------------------------------------------------------------
// ACTIONS
// ---------------------------------------------------------------------------------------------------------------

void CircuitClient::post(String conversationId, String content, String subject, ConversationAddTextItemCB cb) {
    debugln("[Circuit] post: execute Circuit Action ::= [CONVERSATION ADD_TEXT_ITEM]");
    String message =
        TextItem::buildConversationAddTextItemRequest(_requestCounter, conversationId, "", content, subject);
    if (cb != NULL) {
        OpenResponse openResponse = OpenResponse(_requestCounter, OpenResponse::CREATE_TEXT_ITEM);
        openResponse.conversationAddTextItemCB = cb;
        _openResponses.insert({_requestCounter, openResponse});
    }
    _requestCounter++;
    _webSocket.sendTXT(message);
}

void CircuitClient::reply(String conversationId, String parentItemId, String content, ConversationAddTextItemCB cb) {
    debugln("[Circuit] reply: execute Circuit Action ::= [CONVERSATION ADD_TEXT_ITEM]");
    String message = TextItem::buildConversationAddTextItemRequest(_requestCounter, conversationId, "", content, "");
    if (cb != NULL) {
        OpenResponse openResponse = OpenResponse(_requestCounter, OpenResponse::CREATE_TEXT_ITEM);
        openResponse.conversationAddTextItemCB = cb;
        _openResponses.insert({_requestCounter, openResponse});
    }
    _requestCounter++;
    _webSocket.sendTXT(message);
}

void CircuitClient::subscribeToPresence(std::vector<String> userIds, UserSubscribePresenceCB cb) {
    debugln("[Circuit] subscribeToPresence: execute Circuit Action ::= [USER SUBSCRIBE_PRESENCE]");
    String message = PresenceState::buildUserSubscribePresenceRequest(_requestCounter, userIds);
    if (cb != NULL) {
        OpenResponse openResponse = OpenResponse(_requestCounter, OpenResponse::SUBSCRIBE_PRESENCE);
        openResponse.userSubscribePresenceCB = cb;
        _openResponses.insert({_requestCounter, openResponse});
    }
    _requestCounter++;
    _webSocket.sendTXT(message);
}

// ---------------------------------------------------------------------------------------------------------------
// EVENTING
// ---------------------------------------------------------------------------------------------------------------

void CircuitClient::onConversationAddItemEvent(ConversationAddItemEventCB callback) {
    this->_conversationAddItemEventCB = callback;
}

void CircuitClient::onUserUserPresenceChangedEvent(UserUserPresenceChangedEventCB callback) {
    this->_userUserPresenceChangedEventCB = callback;
}

void CircuitClient::onRTCSessionSessionStartedEvent(RTCSessionSessionStartedEventCB callback) {
    this->_rtcSessionSessionStartedEventCB = callback;
}

}  // namespace Circuit