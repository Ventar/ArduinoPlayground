#ifndef Circuit_Client_h
#define Circuit_Client_h

#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <Hash.h>
#include <WebSockets/WebSockets.h>
#include <WebSockets/WebSocketsClient.h>
#include <map>
#include <vector>
#include "Call.h"
#include "PresenceState.h"
#include "TextItem.h"

namespace Circuit {

/**
 * Result code used in response callbacks from the backend to inform the client about errors or success.
 */
struct ResultCode {
    enum Code { OK, ERROR };

    ResultCode(Code code) { this->code = code; }

    ResultCode(Code code, String message) {
        this->code = code;
        this->message = message;
    }

    Code code;
    String message;
};

/**
 * Base class to interact with the Circuit system. This is the entry point for all access to the Circuit system and its
 * functionalities.
 *
 * @author Michael Rodenbuecher
 * @since 2019-07-17
 */
class CircuitClient {
public:
    /**
     * Creates a new Circuit websocket connection to exchange messages and receive events.
     *
     * @param host the host of the system, e.g. circuitsandbox.net
     * @param port the port of the system, default is 443
     * @param username the username of the user which wants to interact with the system
     * @param password the password of the user. Please keep in mind that this is only a demo libary. In general Circuit
     *                 username and password should be kept secret and not entered into any tool.
     */
    CircuitClient(String host, uint16_t port, String username, String password);

    /**
     * Adds an optional proxy configuration in case this client is executed within a company network.
     */
    void setProxy(String proxyHost, uint16_t proxyPort);

    /**
     *  Setup method used to initialize the component
     */
    void setup();

    /**
     * Logic executed during every loop() in the main programm.
     */
    void loop();

    // -------------------------------------------------------------------------------------------------------------
    // Actions
    // -------------------------------------------------------------------------------------------------------------

    // ---- MESSAGING ----------------------------------------------------------------------------------------------

    // Circuit uses a proprietary messaging protocol on top of the async websocket protocol. If the client sends
    // a request to the Circuit backend it will always receive a response with a result code and if needed additional
    // data. Due to the async nature we cannot return the result immediately but need to register a callback that is
    // triggered when the response from the backend arrives.
    // However in some cases you may not be interested in the result, especially for some simple demo cases. Therefore
    // the following methods to trigger business logic do offer two signatures, one with and one without a callback.

    /**
     *  Callback for acknowledge replies, i.e. replies which are either OK or ERROR, but do not contain additional
     * content.
     *  @param resultCode the result code.
     */
    typedef std::function<void(ResultCode resultCode)> AcknowledgeCB;

    /**
     *  Callback for subscribe presence replies.
     *  @param resultCode the result code.
     *  @param states the current presence states of the users for which the subscription was done.
     */
    typedef std::function<void(ResultCode resultCode, std::vector<PresenceState> states)> UserSubscribePresenceCB;

    /**
     *  Callback for posts and replies in Circuit.
     *  @param resultCode the result code.
     *  @param item the created item. Will contain the unique item id, creation and modification time on top of the
     *       send data. Can be used for further processing if needed.
     */
    typedef std::function<void(ResultCode resultCode, TextItem item)> ConversationAddTextItemCB;

    /**
     * Creates a new text item in Circuit for the authorized user. This is a new post that can have multiple replies.
     *
     * @param conversationId the unique ID of the conversation to which the item will be attached (posted)
     * @param content the content / text to post
     * @param subject the subject of the item (optional)
     * @param cb the callback to trigger when the response from the circuit backend is received
     */
    void post(String conversationId, String content, String subject, ConversationAddTextItemCB cb = NULL);

    /**
     * Creates a new text item in Circuit for the authorized user. This is a new reply that was send for a given
     * post. Replies cannot have a subject.
     *
     * @param conversationId the unique ID of the conversation to which the item will be attached (posted)
     * @param parentItemId the unique ID of the parent item (post) to which the reply should be attached.
     * @param content the content / text to post
     * @param cb the callback to trigger when the response from the circuit backend is received
     */
    void reply(String conversationId, String parentItemId, String content, ConversationAddTextItemCB cb = NULL);

    /**
     * Subscribes to a list of user Ids for presence state change ecents. As a result presence state change
     *  events will be triggered with the new presence state of the user.
     * @param userIds the ids to subscribe to.
     * @param cb the callback to trigger when the response from the circuit backend is received
     */
    void subscribeToPresence(std::vector<String> userIds, UserSubscribePresenceCB cb = NULL);

    /**
     * Collects information about open respones to trigger corresponding callback functions.
     */
    struct OpenResponse {
        enum OpenResponseType { CREATE_TEXT_ITEM, SUBSCRIBE_PRESENCE };

        OpenResponse(uint64_t requestId, OpenResponseType type) {
            this->requestId = requestId;
            this->type = type;
        }

        /**
         * The request ID which is used to match the response to the corresponding request
         */
        uint64_t requestId;

        /**
         * The type of the response. Based on the type the additonal callbacks in the class are filled.
         */
        OpenResponseType type;

        AcknowledgeCB acknowledgeCB = NULL;
        ConversationAddTextItemCB conversationAddTextItemCB = NULL;
        UserSubscribePresenceCB userSubscribePresenceCB = NULL;
    };

    // -------------------------------------------------------------------------------------------------------------
    // Events
    // -------------------------------------------------------------------------------------------------------------

    // ---- MESSAGING ----------------------------------------------------------------------------------------------

    typedef std::function<void(TextItem item)> ConversationAddItemEventCB;
    typedef std::function<void(PresenceState state)> UserUserPresenceChangedEventCB;
    typedef std::function<void(Call call)> RTCSessionSessionStartedEventCB;

    /**
     * Event triggered when a new text item event was received, either because of an update or an creation.
     *
     * @param callback event the callback to execute
     */
    void onConversationAddItemEvent(ConversationAddItemEventCB callback);

    /**
     * Event triggered when the presence state of a user changed.
     *
     * @param callback event the callback to execute
     */
    void onUserUserPresenceChangedEvent(UserUserPresenceChangedEventCB callback);

    /**
     * Event triggered when a new session was started.
     *
     * @param callback event the callback to execute
     */
    void onRTCSessionSessionStartedEvent(RTCSessionSessionStartedEventCB callback);

private:
    /**
     * The logon process to Circuit is a two step one. First we need to generate a cookie via an HTTP call and
     * afterwards this cookie has to be used to authenticate during the websocket upgrade.
     */
    void createAccessServerCookie();

    /**
     * Websocket callback function to handle incoming WS messages. Covers the handling of connections and disconnetion,
     * errors and the receive of regular text frames.
     */
    void onWebSocketEvent(WStype_t type, uint8_t* payload, size_t length);

    /**
     * Handles a valid text frame from the Circuit system of type RESPONSE.
     *
     * @param message the JSON document representing the message
     */
    void handleWSResponse(DynamicJsonDocument& message);

    /**
     * Handles a valid text frame from the Circuit system of type EVENT.
     *
     * @param message the JSON document representing the message
     */
    void handleWSEvent(DynamicJsonDocument& message);

    /**
     * Send a message to the Circuit backend via the websocket connection.
     */
    void sendWSMessage(String message);

    /**
     * The Circuit host to connect to.
     */
    String _host;

    /**
     * The port, default is 443;
     */
    uint16_t _port;

    /**
     * An optional host for a company proxy.
     */
    String _proxyHost;

    /**
     * Corresponding port for the otional company proxy.
     */
    uint16_t _proxyPort;

    /**
     * The cookie generated by the createAccessServerCookie() method to authenticate during the websocket upgrade.
     */
    String _wsCookie;

    /**
     * Circuit username.
     */
    String _username;

    /**
     * Circuit password
     */
    String _password;

    /**
     * Counter for requests send out to the backend. Used to match responses to requests.
     */
    unsigned long _requestCounter = 1;

    /**
     * Holds a list of open responses to trigger the corresponding callbacks.
     */
    std::map<uint64_t, OpenResponse> _openResponses;

    /**
     * The websocket client used to send messageds and receive events to the Circuit system.
     */
    WebSocketsClient _webSocket;

    /**
     * Event callback for a new or updated text item.
     */
    ConversationAddItemEventCB _conversationAddItemEventCB;

    /**
     * Event callback for user presence state changes.
     */
    UserUserPresenceChangedEventCB _userUserPresenceChangedEventCB;

    /**
     * Event callback for user rtc session session started event.
     */
    RTCSessionSessionStartedEventCB _rtcSessionSessionStartedEventCB;
};

}  // namespace Circuit
#endif