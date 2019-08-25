
#ifndef AsyncNeoPixel_h
#define AsyncNeoPixel_h

#include <Adafruit_NeoPixel.h>
#include <Animation.h>
#include <vector>

namespace leds {

/**
 * Supported modes for easier use in none web related programs
 */
typedef enum {
  OFF,
  STATIC,
  CIRCLE,
  CLOUD,
  FADE_IN_OUT,
  FIRE,
  METEOR_RAIN,
  RAINBOW,
  THEATER_CHASE
} AsyncNeoPixelMode;

class AsyncNeoPixel {

public:
  /**
   * Constructor.
   *
   * @param numberOfPixels Number of NeoPixels in strand.
   * @param pin Arduino pin number which will drive the NeoPixel data in.
   * @param pixelType Pixel type -- add together NEO_* constants defined in
   *           Adafruit_NeoPixel.h, for example NEO_GRB+NEO_KHZ800 for
   * NeoPixels expecting an 800 KHz (vs 400 KHz) data stream with color
   * bytes expressed in green, red, blue order per pixel.
   */
  AsyncNeoPixel(uint16_t numberOfPixels, uint8_t pin, neoPixelType pixelType);

  /**
   * Perform the setup operation of this class inside of the Arduino setup
   * method.
   */
  void setup();

  /**
   * Execute the logic during an Arduino loop cycle.
   * */
  void loop();

  /**
   * Set the mode of the LED strip. The mode defines if the strip is off,
   * displays single colors or special effects.
   */
  void setMode(String modeName);

  /**
   * Set the mode of the LED strip. The mode defines if the strip is off,
   * displays single colors or special effects.
   */
  void setMode(AsyncNeoPixelMode mode);

  /**
   * Set the mode of the LED strip. The mode defines if the strip is off,
   * displays single colors or special effects.
   */
  void setMode(String modeName, String colors);

  /**
   * Set the mode of the LED strip. The mode defines if the strip is off,
   * displays single colors or special effects.
   */
  void setMode(AsyncNeoPixelMode mode, String colors);

  /**
   * Set the mode of the LED strip. The mode defines if the strip is off,
   * displays single colors or special effects.
   */
  void setMode(String modeName, String colors, String speed);

  /**
   * Set the mode of the LED strip. The mode defines if the strip is off,
   * displays single colors or special effects.
   */
  void setMode(AsyncNeoPixelMode mode, String colors, String speed);

  /**
   * Returns the animation object of the currently active animation.
   * */
  Animation *getAnimation();

  /**
   * Returns an array of animations.
   * */
  std::vector<Animation *> getAnimations();

  /**
   * Returns the number of pixels.
   */
  uint16_t numPixels();

private:
  /**
   * Converts a mode enum to a corresponding String.
   */
  String modeToString(AsyncNeoPixelMode mode);

  /**
   * Number of NeoPixels in strand.
   */
  uint16_t numberOfPixels;
  /**
   * Arduino pin number which will drive the NeoPixel data in.
   */
  uint8_t pin;
  /**
   * Pixel type -- add together NEO_* constants defined in
   * Adafruit_NeoPixel.h,for example NEO_GRB+NEO_KHZ800 for NeoPixels expecting
   * an 800 KHz (vs 400 KHz) data stream with color bytes expressed in green,
   * red, blue order per pixel.
   */
  neoPixelType pixelType;
  /**
   * Reference to the NeoPixel strip.
   */
  Adafruit_NeoPixel *strip;
  /**
   * The current mode of the animation.
   */
  String modeName = "Off";
  /**
   * A vector with the available animations.
   */
  std::vector<Animation *> animations;
};

} // namespace leds

#endif