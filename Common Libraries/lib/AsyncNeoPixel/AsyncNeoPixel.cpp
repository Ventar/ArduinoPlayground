#include <AnimationCircle.h>
#include <AnimationCloud.h>
#include <AnimationDoubleCircle.h>
#include <AnimationFadeInOut.h>
#include <AnimationFire.h>
#include <AnimationMeteorRain.h>
#include <AnimationOff.h>
#include <AnimationRainbow.h>
#include <AnimationSparkle.h>
#include <AnimationStatic.h>
#include <AnimationTheaterChase.h>
#include <AsyncNeoPixel.h>

namespace leds {

String AsyncNeoPixel::modeToString(AsyncNeoPixelMode mode) {
  switch (mode) {
  case OFF:
    return "Off";
  case STATIC:
    return "Static";
  case CIRCLE:
    return "Circle";
  case CLOUD:
    return "Cloud";
  case FADE_IN_OUT:
    return "Fade In Out";
  case FIRE:
    return "Fire";
  case METEOR_RAIN:
    return "Meteor Rain";
  case RAINBOW:
    return "Rainbow";
  case THEATER_CHASE:
    return "Theater Chase";
  }

  return "Static";
}

AsyncNeoPixel::AsyncNeoPixel(uint16_t n, uint8_t p, neoPixelType t) {
  this->numberOfPixels = n;
  this->pin = p;
  this->pixelType = t;
}

void AsyncNeoPixel::setup() {
  // strip = new Adafruit_NeoPixel(14, D5, NEO_GRB + NEO_KHZ800);
  strip = new Adafruit_NeoPixel(numberOfPixels, pin, pixelType);

  this->animations.reserve(10);
  this->animations.push_back(new AnimationOff(strip, 20000, 50));
  this->animations.push_back(new AnimationStatic(strip, 20000, 50));
  this->animations.push_back(new AnimationFadeInOut(strip, 20000, 50));
  this->animations.push_back(new AnimationRainbow(strip, 20000, 50));
  this->animations.push_back(new AnimationFire(strip, 20000, 50));
  this->animations.push_back(new AnimationMeteorRain(strip, 20000, 50));
  this->animations.push_back(new AnimationTheaterChase(strip, 20000, 50));
  this->animations.push_back(new AnimationSparkle(strip, 20000, 50));
  this->animations.push_back(new AnimationCircle(strip, 20000, 50));
  this->animations.push_back(new AnimationCloud(strip, 20000, 50));
  //  this->animations.push_back(new AnimationDoubleCircle(&strip));

  strip->begin();
  for (int i = 0; i < strip->numPixels(); i++) {
    strip->setPixelColor(i, 0);
  }
  strip->show();
}

uint16_t AsyncNeoPixel::numPixels() { return strip->numPixels(); }

void AsyncNeoPixel::setMode(AsyncNeoPixelMode mode, String colors,
                            String speed) {
  setMode(modeToString(mode), colors, speed);
}

void AsyncNeoPixel::setMode(String modeName, String colors, String speed) {
  for (size_t i = 0; i < this->animations.size(); i++) {
    if (animations.at(i)->getSceneData()->modeName == modeName) {
      this->modeName = modeName;

      if (colors != "") {
        animations.at(i)->setColorListFromString(colors);
      }

      if (speed != "") {
        animations.at(i)->setSpeed(speed.toInt());
      }

      /*
      if (colors.length() >= 21) {
          Serial.printf(
              "AsyncNeoPixel::setMode - mode ::= [%s], delay ::= [%d], speed
      ::=[%d], " "colors ::= [%s]\n",
              animations.at(i)->getSceneData()->modeName.c_str(),
      animations.at(i)->getSceneData()->delay,
              animations.at(i)->getSceneData()->speed, (colors.substring(0, 21)
      + String("...")).c_str()); } else { Serial.printf( "AsyncNeoPixel::setMode
      - mode ::= [%s], delay ::= [% d], speed :: = [% d],  colors :: = [% s]\n
      ", animations.at(i)->getSceneData()->modeName.c_str(),
      animations.at(i)->getSceneData()->delay,
              animations.at(i)->getSceneData()->speed, colors.c_str());
      }
      */

      animations.at(i)->reset();
    }
  }
}

void AsyncNeoPixel::setMode(String modeName, String colors) {
  setMode(modeName, colors, "");
}

void AsyncNeoPixel::setMode(AsyncNeoPixelMode mode, String colors) {
  setMode(modeToString(mode), colors, "");
}

void AsyncNeoPixel::setMode(String modeName) { setMode(modeName, "", ""); }

void AsyncNeoPixel::setMode(AsyncNeoPixelMode mode) {
  setMode(modeToString(mode), "", "");
}

void AsyncNeoPixel::loop() {
  for (size_t i = 0; i < this->animations.size(); i++) {
    if (animations.at(i)->getSceneData()->modeName == modeName) {
      animations.at(i)->update();
    }
  }
}

Animation *AsyncNeoPixel::getAnimation() {
  for (size_t i = 0; i < this->animations.size(); i++) {
    if (animations.at(i)->getSceneData()->modeName == modeName) {
      return animations.at(i);
    }
  }
  return NULL;
}

std::vector<Animation *> AsyncNeoPixel::getAnimations() { return animations; }

} // namespace leds