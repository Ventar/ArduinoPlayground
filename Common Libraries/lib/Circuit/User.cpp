#include "User.h"

namespace Circuit {

User::User() {}

// -------------------------------------------------------------------------------------------------------------
// Events
// -------------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------------
// Request / Response
// -------------------------------------------------------------------------------------------------------------

/*
{
   "msgType": "REQUEST",
   "request": {
      "requestId": 232,
      "type": "USER",
      "user": {
         "type": "GET_USERS_BY_IDS",
         "usersByIds": {
            "userIds": [
               "d94a4f77-1cb9-49ba-98c3-cd44d6da1e39"
            ],
            "complete": true
         }
      }
   }
}
 */
String User::buildUserGetUsersByIdsRequest(uint64_t requestId, std::vector<String> userIds) {
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
}  // namespace Circuit

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
std::vector<User> User::fromUserGetUsersByIdsResponse(DynamicJsonDocument& message) {
    std::vector<User> user;

    // mandatory fields
    /*
      for (JsonObject state : message["response"]["user"]["subscribePresence"]["states"].as<JsonArray>()) {
          states.push_back(PresenceState(String((const char*)state["userId"]), String((const char*)state["state"])));
      };
      */

    return user;
}

// String buildGetPresenceRequest(std::vector<String> userIds) { return ""; }
// std::vector<PresenceState> fromGetPresenceStateResponse(DynamicJsonDocument& message) { return; }

// -------------------------------------------------------------------------------------------------------------
// Common
// -------------------------------------------------------------------------------------------------------------

String User::toString() { return "User [userId=" + userId + "]"; }

}  // namespace Circuit