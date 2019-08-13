#ifndef Circuit_Call_h
#define Circuit_Call_h

#include <Arduino.h>
#include <ArduinoJson.h>

namespace Circuit {

/**
 * Wrapper aroung a Circuit text item to make handling easier.
 */
class Call {
  public:
    /**
     * Creates a new Call item.
     */
    Call();

    /**
     * Creates a new call item from the JSON definition passed to the method.
     * @param the JSON deinition of a call item.
     */
    static Call fromRTCSessionStartedEvent(DynamicJsonDocument& message);

    // -------------------------------------------------------------------------------------------------------------
    // Request / Response
    // -------------------------------------------------------------------------------------------------------------

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
    String conversationId;

};

}  // namespace Circuit

#endif