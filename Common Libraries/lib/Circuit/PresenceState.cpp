#include "PresenceState.h"

namespace Circuit {

PresenceState::PresenceState() {}

PresenceState::PresenceState(String userId, String state) {
    this->userId = userId;
    this->presenceState = state;
}

// -------------------------------------------------------------------------------------------------------------
// Events
// -------------------------------------------------------------------------------------------------------------

/*
{
  "apiVersion":"2.9.164-SNAPSHOT",
  "msgType":"EVENT",
  "clientId":"34c9cb29-d648-47ab-a922-8cb1efbbc4c8",
  "event":{
    "uuid":"f80f3f69-04e6-4533-a5b4-6a904219ab96",
    "serialNumber":1,
    "type":"USER",
    "user":{
      "type":"USER_PRESENCE_CHANGE",
      "userId":"b1fa63df-45b3-4b7e-8651-27d6beabfbcd",
      "presenceChanged":{
        "newState":{
          "userId":"b1fa63df-45b3-4b7e-8651-27d6beabfbcd",
          "state":"AVAILABLE",
          "inTransit":false,
          "mobile":false,
          "poor":false,
          "isOptedOut":false
        },
        "username":"Alper Teke"
      }
    }
  }
}
*/
PresenceState PresenceState::fromUserUserPresenceChangeEvent(DynamicJsonDocument& message) {
    PresenceState item;

    // mandatory fields
    item.userId = String((const char*)message["event"]["user"]["userId"]);
    item.presenceState = String((const char*)message["event"]["user"]["presenceChanged"]["newState"]["state"]);

    return item;
}

// -------------------------------------------------------------------------------------------------------------
// Request / Response
// -------------------------------------------------------------------------------------------------------------

/*
{
  "msgType":"REQUEST",
  "request":{
    "requestId":3,
    "type":"USER",
    "user":{
      "type":"SUBSCRIBE_PRESENCE",
      "subscribePresence":{
        "userIds":[
          "b1fa63df-45b3-4b7e-8651-27d6beabfbcd"
        ]
      }
    }
  },
  "clientId":"34c9cb29-d648-47ab-a922-8cb1efbbc4c8"
}
 */
String PresenceState::buildUserSubscribePresenceRequest(uint64_t requestId, std::vector<String> userIds) {
    DynamicJsonDocument doc(1024);
    doc["msgType"] = "REQUEST";

    JsonObject request = doc.createNestedObject("request");
    request["requestId"] = requestId;
    request["type"] = "USER";

    JsonObject user = request.createNestedObject("user");
    user["type"] = "SUBSCRIBE_PRESENCE";

    JsonObject subscribePresence = user.createNestedObject("subscribePresence");
    JsonArray states = subscribePresence.createNestedArray("userIds");

    for (auto& id : userIds) {
        states.add(id);
    }

    String output = "";
    serializeJson(doc, output);
    return output;
}

/*
{
  "apiVersion":"2.9.164-SNAPSHOT",
  "msgType":"RESPONSE",
  "clientId":"34c9cb29-d648-47ab-a922-8cb1efbbc4c8",
  "response":{
    "requestId":3,
    "code":"OK",
    "appNodeName":"lap-mro-3",
    "type":"USER",
    "user":{
      "type":"SUBSCRIBE_PRESENCE",
      "subscribePresence":{
        "states":[
          {
            "userId":"b1fa63df-45b3-4b7e-8651-27d6beabfbcd",
            "state":"OFFLINE",
            "inTransit":false,
            "mobile":false,
            "poor":false,
            "isOptedOut":false
          }
        ]
      }
    }
  }
}
 */
std::vector<PresenceState> PresenceState::fromUserSubscribePresenceResponse(DynamicJsonDocument& message) {
    std::vector<PresenceState> states;

    // mandatory fields
    for (JsonObject state : message["response"]["user"]["subscribePresence"]["states"].as<JsonArray>()) {
        states.push_back(PresenceState(String((const char*)state["userId"]), String((const char*)state["state"])));
    };

    return states;
}

// String buildGetPresenceRequest(std::vector<String> userIds) { return ""; }
// std::vector<PresenceState> fromGetPresenceStateResponse(DynamicJsonDocument& message) { return; }

// -------------------------------------------------------------------------------------------------------------
// Common
// -------------------------------------------------------------------------------------------------------------

String PresenceState::toString() { return "PresenceState [userId=" + userId + ", presenceState=" + presenceState + "]"; }

}  // namespace Circuit