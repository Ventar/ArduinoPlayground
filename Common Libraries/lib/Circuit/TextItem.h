#ifndef Circuit_TextItem_h
#define Circuit_TextItem_h

#include <Arduino.h>
#include <ArduinoJson.h>

namespace Circuit {

/**
 * Wrapper aroung a Circuit text item to make handling easier.
 */
class TextItem {
  public:
    /**
     * Creates a new text item from the document passed if the structure is valid
     *
     * @param the JSON representation of a text item
     */
    TextItem();

    /**
     * Creates a new text item from the JSON definition passed to the method.
     * @param the JSON deinition of a text item.
     */
    static TextItem fromConversationAddItemEvent(DynamicJsonDocument& message);

    // -------------------------------------------------------------------------------------------------------------
    // Request / Response
    // -------------------------------------------------------------------------------------------------------------

    /**
     * Creates a new request to send it to the Circuit backend.
     */
    static String buildConversationAddTextItemRequest(uint64_t requestId, String conversationId, String parentItemId, String content, String subject);

    /**
     * Creates a new text item from the JSON definition passed to the method.
     * @param the JSON deinition of a text item.
     */
    static TextItem fromConversationAddTextItemResponse(DynamicJsonDocument& message);

    // -------------------------------------------------------------------------------------------------------------
    // DATA
    // -------------------------------------------------------------------------------------------------------------

    /**
     * Converts the current text item to it's string representation.
     */
    String toString();

    /**
     * The unique item id of the text item.
     */
    String itemId;

    /**
     * The unique id of the parent item. If this id is filled the text item is a reply to a previous posted item and
     * cannot have a subject.
     */
    String parentItemId;

    /**
     * The unique conversation ID of the conversation to which the item belongs.
     */
    String conversationId;

    /**
     * The subject of the item.
     */
    String subject;

    /**
     * The actual content of the text item.
     */
    String content;

    /**
     * The unique user Id of the creator of this item.
     */
    String creatorId;

    /**
     * The time when this item was created.
     */
    uint64_t creationTime;

    /**
     * The time when this item was modified
     */
    uint64_t modificationTime;
};

}  // namespace Circuit

#endif