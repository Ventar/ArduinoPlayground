#include "TextItem.h"

namespace Circuit {

TextItem::TextItem() {}

// -------------------------------------------------------------------------------------------------------------
// Events
// -------------------------------------------------------------------------------------------------------------

/*
{
  "apiVersion":"2.9.164-SNAPSHOT",
  "msgType":"EVENT",
  "clientId":"9778f6a5-a3b4-4fcd-8a28-93c2ad759fc6",
  "event":{
    "uuid":"d06906dc-be54-4293-a6ea-1dc3fa8527a5",
    "serialNumber":3,
    "type":"CONVERSATION",
    "conversation":{
      "type":"ADD_ITEM",
      "convId":"b88609d3-9330-4266-9bbb-7e1263771e39",
      "addItem":{
        "item":{
          "type":"TEXT",
          "itemId":"b0b02372-fbea-4007-9071-dea7781b3d5b",
          "convId":"b88609d3-9330-4266-9bbb-7e1263771e39",
          "text":{
            "state":"CREATED",
            "contentType":"RICH",
            "content":"****",
            "webhookmessage":false
          },
          "creationTime":1563650235698,
          "modificationTime":1563650235698,
          "creatorId":"5d97c2f3-fb5f-4105-b73c-ea8abee54ea6",
          "includeInUnreadCount":true,
          "dataRetentionState":"UNTOUCHED",
          "hasDecryptionError":false
        },
        "conversationType":"GROUP"
      }
    }
  }
}
*/
TextItem TextItem::fromConversationAddItemEvent(DynamicJsonDocument& message) {
    TextItem item;

    // mandatory fields
    item.itemId = String((const char*)message["event"]["conversation"]["addItem"]["item"]["itemId"]);
    item.creatorId = String((const char*)message["event"]["conversation"]["addItem"]["item"]["creatorId"]);
    item.conversationId = String((const char*)message["event"]["conversation"]["addItem"]["item"]["convId"]);
    item.creationTime = message["event"]["conversation"]["addItem"]["item"]["creationTime"];
    item.modificationTime = message["event"]["conversation"]["addItem"]["item"]["modificationTime"];

    // optional fields
    if (!message["event"]["conversation"]["addItem"]["item"]["text"]["content"].isNull())
        item.content = String((const char*)message["event"]["conversation"]["addItem"]["item"]["text"]["content"]);

    if (!message["event"]["conversation"]["addItem"]["item"]["text"]["parentItemId"].isNull())
        item.parentItemId = String((const char*)message["event"]["conversation"]["addItem"]["item"]["text"]["parentItemId"]);

    if (!message["event"]["conversation"]["addItem"]["item"]["text"]["subject"].isNull())
        item.subject = String((const char*)message["event"]["conversation"]["addItem"]["item"]["text"]["subject"]);

    return item;
}

// -------------------------------------------------------------------------------------------------------------
// Request / Response
// -------------------------------------------------------------------------------------------------------------

/*
{
  "msgType":"REQUEST",
  "request":{
    "requestId":1,
    "type":"CONVERSATION",
    "conversation":{
      "type":"ADD_TEXT_ITEM",
      "addTextItem":{
        "convId":"b88609d3-9330-4266-9bbb-7e1263771e39",
        "content":"abc",
        "contentType":"RICH"
      }
    }
  },
  "clientId":"78b02b27-3cd8-4ef0-963f-7ec3d9a4d7e5"
}
 */
String TextItem::buildConversationAddTextItemRequest(uint64_t requestId, String conversationId, String parentItemId, String content, String subject) {
    DynamicJsonDocument doc(512 + content.length());
    doc["msgType"] = "REQUEST";

    JsonObject request = doc.createNestedObject("request");
    request["requestId"] = requestId;
    request["type"] = "CONVERSATION";

    JsonObject conversation = request.createNestedObject("conversation");
    conversation["type"] = "ADD_TEXT_ITEM";

    JsonObject addTextItem = conversation.createNestedObject("addTextItem");
    addTextItem["convId"] = conversationId;
    addTextItem["content"] = content;
    addTextItem["contentType"] = "RICH";

    if (parentItemId != "") {
        addTextItem["parentId"] = parentItemId;
    }

    if (subject != "") {
        addTextItem["subject"] = subject;
    }

    String output = "";
    serializeJson(doc, output);
    return output;
}

/*
{
  "apiVersion":"2.9.164-SNAPSHOT",
  "msgType":"RESPONSE",
  "clientId":"78b02b27-3cd8-4ef0-963f-7ec3d9a4d7e5",
  "response":{
    "requestId":1,
    "code":"OK",
    "appNodeName":"lap-mro-3",
    "type":"CONVERSATION",
    "conversation":{
      "type":"ADD_TEXT_ITEM",
      "addTextItem":{
        "item":{
          "type":"TEXT",
          "itemId":"d9a08a72-1de5-4368-9406-22afe4c3df08",
          "convId":"b88609d3-9330-4266-9bbb-7e1263771e39",
          "text":{
            "state":"CREATED",
            "contentType":"RICH",
            "content":"****",
            "webhookmessage":false
          },
          "creationTime":1563631410823,
          "modificationTime":1563631410823,
          "creatorId":"5d97c2f3-fb5f-4105-b73c-ea8abee54ea6",
          "includeInUnreadCount":true,
          "dataRetentionState":"UNTOUCHED",
          "hasDecryptionError":false
        }
      }
    }
  }
}
*/
TextItem TextItem::fromConversationAddTextItemResponse(DynamicJsonDocument& message) {
    TextItem item;

    // mandatory fields
    item.itemId = String((const char*)message["response"]["conversation"]["addTextItem"]["item"]["itemId"]);
    item.creatorId = String((const char*)message["response"]["conversation"]["addTextItem"]["item"]["creatorId"]);
    item.conversationId = String((const char*)message["response"]["conversation"]["addTextItem"]["item"]["convId"]);
    item.creationTime = message["response"]["conversation"]["addTextItem"]["item"]["creationTime"];
    item.modificationTime = message["response"]["conversation"]["addTextItem"]["item"]["modificationTime"];

    // optional fields
    if (!message["response"]["conversation"]["addTextItem"]["item"]["text"]["content"].isNull())
        item.content = String((const char*)message["response"]["conversation"]["addTextItem"]["item"]["text"]["content"]);

    if (!message["response"]["conversation"]["addTextItem"]["item"]["text"]["parentItemId"].isNull())
        item.parentItemId = String((const char*)message["response"]["conversation"]["addTextItem"]["item"]["text"]["parentItemId"]);

    if (!message["response"]["conversation"]["addTextItem"]["item"]["text"]["subject"].isNull())
        item.subject = String((const char*)message["response"]["conversation"]["addTextItem"]["item"]["text"]["subject"]);

    return item;
}

// -------------------------------------------------------------------------------------------------------------
// Common
// -------------------------------------------------------------------------------------------------------------

String TextItem::toString() {
    return "TextItem [itemId=" + itemId + ", creatorId=" + creatorId + ", conversationId=" + conversationId + ", parentItemId=" + parentItemId +
           ",creationTime=" + String((unsigned long)creationTime) + ", modificationTime=" + String((unsigned long)modificationTime) + "]";
}

}  // namespace Circuit