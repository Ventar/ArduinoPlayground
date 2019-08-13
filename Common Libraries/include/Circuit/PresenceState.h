#ifndef Circuit_PresenceState_h
#define Circuit_PresenceState_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>

namespace Circuit {

/**
 * Wrapper aroung a Circuit presence state to make handling easier.
 */
class PresenceState {
  public:
    /**
     * Creates a new presence state.
     *
     */
    PresenceState();

    /**
     * Creates a new presence state.
     *
     */
    PresenceState(String userId, String state);

    // -------------------------------------------------------------------------------------------------------------
    // Events
    // -------------------------------------------------------------------------------------------------------------

    /**
     * Creates a new presence from the JSON definition passed to the method.
     *
     * @param the JSON definition of a presence state changed event.
     */
    static PresenceState fromUserUserPresenceChangeEvent(DynamicJsonDocument& message);

    // -------------------------------------------------------------------------------------------------------------
    // Request / Response
    // -------------------------------------------------------------------------------------------------------------
  
    /**
     * Creates a new request to send it to the Circuit backend to subscribe to presence state
     * changed events for the given user ids.
     *
     * @param userIds the user ids to subscribe to
     */
    static String buildUserSubscribePresenceRequest(uint64_t requestId, std::vector<String> userIds);

    /**
     * Creates a new list of presence states from the result.
     * @param the JSON definition for a get presence state response.
     */
    static std::vector<PresenceState> fromUserSubscribePresenceResponse(DynamicJsonDocument& message);

    /**
     * Creates a new request to send it to the Circuit backend to fetch the presence states
     * for the given user ids.
     *
     * @param userIds the user ids to fetch the presence state for
     */
    // static String buildGetPresenceRequest(std::vector<String> userIds);

    /**
     * Creates a new list of presence states from the result.
     * @param the JSON definition for a get presence state response.
     */
    // static std::vector<PresenceState> fromGetPresenceStateResponse(DynamicJsonDocument& message);

    // -------------------------------------------------------------------------------------------------------------
    // Common
    // -------------------------------------------------------------------------------------------------------------

    /**
     * Converts the current text item to it's string representation.
     */
    String toString();

    /**
     * The presence state.
     */
    String presenceState;

    /**
     * The unique user Id of the iser to which the presence state belongs.
     */
    String userId;
};

}  // namespace Circuit

#endif