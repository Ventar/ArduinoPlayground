#include "Call.h"

namespace Circuit {

Call::Call() {}

// -------------------------------------------------------------------------------------------------------------
// Events
// -------------------------------------------------------------------------------------------------------------

/*
{
  "apiVersion":"2.9.166",
  "msgType":"EVENT",
  "clientId":"e42d46d8-609c-4894-83d2-909d50214296",
  "event":{
    "uuid":"fab7086c-996c-431a-8627-efd7e6418627",
    "serialNumber":3,
    "type":"RTC_SESSION",
    "rtcSession":{
      "type":"SESSION_STARTED",
      "sessionId":"2e25332e-cd2c-45a8-b10b-e864e8e726e6",
      "instanceId":"2e25332e-cd2c-45a8-b10b-e864e8e726e6_1565626434404",
      "sessionStarted":{
        "session":{
          "rtcSessionId":"2e25332e-cd2c-45a8-b10b-e864e8e726e6",
          "instanceId":"2e25332e-cd2c-45a8-b10b-e864e8e726e6_1565626434404",
          "convId":"90736477-38f4-4300-962c-01d20403f777",
          "startTime":0,
          "participants":[
            {
              "userId":"94737ebb-c9a7-4b15-9a0f-4ec281d57f1f",
              "clientId":"aa92ae7d-ecf4-48c0-ad4c-70434cba0ef9",
              "clientDisplayName":"1b66c273-3562-470f-9de7-7d70f56df437",
              "muted":false,
              "isExternal":false,
              "participantType":"USER",
              "userDisplayName":"Assassins Minion",
              "pullCall":false,
              "changeMedia":false,
              "isModerator":false,
              "screenSharePointerSupported":false,
              "isMeetingGuest":false,
              "rtcSupportedFeatures":[
                "MID_STREAM_MAPPING",
                "SCREEN_CONTROLLER",
                "UPGRADE_TO_CONFERENCE",
                "CLIENT_SUPPORTING_ASYNC"
              ]
            }
          ],
          "mediaTypes":[
            "AUDIO"
          ],
          "locked":false,
          "muted":false,
          "callOut":true,
          "turnServers":[
            "turn:sandboxturn01.circuitsandbox.net:3481?transport=udp",
            "turn:sandboxturn01.circuitsandbox.net:443?transport=tcp",
            "turns:sandboxturn01.circuitsandbox.net:443?transport=tcp"
          ],
          "hosted":false,
          "macNode":"mac-01-boca-us",
          "appNode":"app-01-boca-us",
          "participantCount":0,
          "maxParticipantCount":0,
          "terminationTime":0,
          "terminationDelay":0,
          "attendeeCount":0,
          "maxAttendeeCount":0,
          "whiteBoardEnabled":false,
          "testMode":false,
          "pollEnabled":false
        },
        "convId":"90736477-38f4-4300-962c-01d20403f777",
        "callOut":true,
        "isTelephony":false
      }
    }
  }
}
*/
Call Call::fromRTCSessionStartedEvent(DynamicJsonDocument& message) {
    Call call;

    // mandatory fields
    call.conversationId = String((const char*)message["event"]["rtcSession"]["sessionStarted"]["session"]["convId"]);

    return call;
}

// -------------------------------------------------------------------------------------------------------------
// Request / Response
// -------------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------------
// Common
// -------------------------------------------------------------------------------------------------------------

String Call::toString() { return "Call [conversationId=" + conversationId + "]"; }

}  // namespace Circuit