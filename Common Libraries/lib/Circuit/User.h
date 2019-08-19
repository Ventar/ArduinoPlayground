#ifndef Circuit_User_h
#define Circuit_User_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <map>
#include <vector>

namespace Circuit {

/**
 * Wrapper aroung a Circuit text item to make handling easier.
 */
class User {
public:
    /**
     * Creates a new Call item.
     */
    User();

    /**
     * Creates a new call item from the JSON definition passed to the method.
     * @param the JSON deinition of a call item.
     */
    static User fromRTCSessionStartedEvent(DynamicJsonDocument& message);

    // -------------------------------------------------------------------------------------------------------------
    // Request / Response
    // -------------------------------------------------------------------------------------------------------------

    /**
     * Creates a new request to send it to the Circuit backend to fetch user objecs based on their IDs
     *
     * @param requestId the unique id to match the response to the OpenResponse in the client
     */
    static String buildUserGetUsersByIdsRequest(uint64_t requestId, std::vector<String> userIds);

    /**
     * Creates a new user from the JSON definition passed to the method.
     *
     * @param message the JSON deinition of a text item.
     */
    static std::vector<User> fromUserGetUsersByIdsResponse(DynamicJsonDocument& message);

    // -------------------------------------------------------------------------------------------------------------
    // DATA
    // -------------------------------------------------------------------------------------------------------------

    /**
     * Converts the current text item to it's string representation.
     */
    String toString();

    /**
     * The unique conversation ID of the conversation to which the item belongs.
     */
    String userId;

private:
    static std::map<String, User> userCache;
};

}  // namespace Circuit

#endif