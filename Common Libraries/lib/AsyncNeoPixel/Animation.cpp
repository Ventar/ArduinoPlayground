#include <Animation.h>

namespace leds {

/**
 * Names of the LED animation modes
 */
Animation::Animation(Adafruit_NeoPixel *strip, String name, String ledUsageName,
                     String id, uint16_t maxDelay, uint16_t speed) {
  this->strip = strip;
  this->id = id;
  this->maxDelay = maxDelay;
  this->data = new SceneData();
  data->speed = speed;
  data->delay = this->maxDelay - (this->maxDelay * data->speed / 100);
  data->modeName = name;
  data->ledUsageName = ledUsageName;

  data->colors = new uint32_t[strip->numPixels()];
  for (int i = 0; i < strip->numPixels(); i++) {
    data->colors[i] = 0;
  }
}

void Animation::reset() { counter = 0; }

String Animation::getId() { return id; }

SceneData *Animation::getSceneData() { return data; }

// Set a LED color (not yet visible)
void Animation::setPixel(int Pixel, uint32_t color) {
  strip->setPixelColor(Pixel, color);
}

void Animation::setPixel(int Pixel, byte red, byte green, byte blue) {
  strip->setPixelColor(Pixel, strip->Color(red, green, blue));
}

// Set all LEDs to a given color and apply it (visible)
void Animation::setAll(byte red, byte green, byte blue) {
  for (int i = 0; i < strip->numPixels(); i++) {
    setPixel(i, red, green, blue);
  }
}

void Animation::show() { strip->show(); }

uint32_t Animation::separateStringList(String data, uint16_t index,
                                       uint32_t defaultColor) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == ',' || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  if (found > index) {
    return strtol(&data.substring(strIndex[0], strIndex[1])[0], NULL, 16);
  }

  return defaultColor;
}

void Animation::setColorListFromString(String colorList) {
  for (uint16_t i = 0; i < strip->numPixels(); i++) {
    uint32_t newColor = separateStringList(colorList, i, lastColor);
    lastColor = newColor;
    data->colors[i] = lastColor;
  }
}

String Animation::getColorListAsString() {
  String matrix = "";
  for (uint16_t i = 0; i < strip->numPixels(); i++) {
    String hexString = String(data->colors[i], HEX);
    while (hexString.length() < 6)
      hexString = "0" + hexString;
    matrix += hexString;
    if (i < strip->numPixels() - 1) {
      matrix += ",";
    }
  }
  return matrix;
}

void Animation::setSpeed(int speed) {
  tick = 0;
  data->speed = speed;
  data->delay = this->maxDelay - (this->maxDelay * speed / 100);
  if (data->delay == 0) {
    data->delay = 1;
  }
}

void Animation::update() {
  tick++;
  if (tick % data->delay == 0) {
    process();
  }
}

} // namespace leds