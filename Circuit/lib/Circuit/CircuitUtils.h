#ifndef Circuit_Utils_h
#define Circuit_Utils_h

#include <Arduino.h>

namespace Circuit {
/**
 * URL encodes unsafe characters: " < > # % { } | \ ^ ~ [ ] and reserved characters:  / ? : @ =
 * for form data requests
 */
String urlencode(String str) {
    String encodedString = "";
    char c;

    for (uint16_t i = 0; i < str.length(); i++) {
        c = str.charAt(i);
        if (c == '@') {
            encodedString += "%40";
        } else if (c == ';') {
            encodedString += "%3B";
        } else if (c == '/') {
            encodedString += "%2F";
        } else if (c == '?') {
            encodedString += "%3F";
        } else if (c == ':') {
            encodedString += "%3A";
        } else if (c == '=') {
            encodedString += "%3D";
        } else if (c == '&') {
            encodedString += "%26";
        } else if (c == '"') {
            encodedString += "%22";
        } else if (c == '<') {
            encodedString += "%3C";
        } else if (c == '>') {
            encodedString += "%3E";
        } else if (c == '#') {
            encodedString += "%23";
        } else if (c == '{') {
            encodedString += "%7B";
        } else if (c == '}') {
            encodedString += "%7D";
        } else if (c == '|') {
            encodedString += "%7C";
        } else if (c == '\\') {
            encodedString += "%5C";
        } else if (c == '~') {
            encodedString += "%7E";
        } else if (c == '[') {
            encodedString += "%5B";
        } else if (c == ']') {
            encodedString += "%5D";
        } else {
            encodedString += c;
        }
    }
    return encodedString;
}
}  // namespace Circuit

#endif