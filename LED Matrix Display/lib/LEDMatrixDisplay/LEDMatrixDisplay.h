#ifndef LEDMatrixDisplay_h
#define LEDMatrixDisplay_h

#include <Arduino.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <SPI.h>

#define HARDWARE_TYPE
#define MAX_DEVICES 4
#define CLK_PIN D8
#define DATA_PIN D6
#define CS_PIN D7
#define SPEED_TIME 30
#define PAUSE_TIME 1000

/**
 * Wrapper class arround the Matrix display.
 */
class LEDMatrixDisplay {
  /**
   * Callback triggered when the animation is done.
   */
  typedef std::function<void()> AnimationDoneEvent;

 public:
  MD_Parola _display = MD_Parola(MD_MAX72XX::FC16_HW, DATA_PIN, CLK_PIN, CS_PIN, 4);

  void setup(void) {
    delete[] _text;
    _display.begin();
    _display.setIntensity(0);
    _display.setInvert(false);
    _display.displayClear();
    _display.setZoneEffect(0, true, PA_FLIP_UD);
    _display.setZoneEffect(0, true, PA_FLIP_LR);
    
  }
  /**
   * Clear the display.
   */
  void clear() { _display.displayClear(); }

  /**
   * Called from the loop() method in the main class to animate the display.
   */
  void loop(void) {
    if (_display.displayAnimate()) {
      //_display.displayReset();

      if (_animationDoneEvent) {
        _animationDoneEvent();
        _animationDoneEvent = NULL;
      }
    };
  }

  /**
   * Sets the text that is displayed on the display.
   */
  void setTextWithCallback(char* text, textPosition_t align, textEffect_t effectIn, textEffect_t effectOut,
                           AnimationDoneEvent doneEvent, uint16_t speed = 30, uint16_t pause = 1000) {
    _animationDoneEvent = doneEvent;
    setText(text, align, effectIn, effectOut, false, false, speed, pause);
  }

  /**
   * Sets the text that is displayed on the display.
   */
  void setText(String text, textPosition_t align, textEffect_t effectIn, textEffect_t effectOut,
               bool waitForAnimation = false, bool clear = false, uint16_t speed = 30, uint16_t pause = 1000) {
    delete[] _text;
    _text = new char[text.length() + 1];
    text.toCharArray(_text, text.length() + 1);

    _display.displayText(_text, align, speed, pause, effectIn, effectOut);

    if (waitForAnimation) {
      this->waitForAnimation();

      if (clear) {
        _display.displayClear();
      }
    }
  }

 private:
  void waitForAnimation() {
    bool finished = false;
    while (!finished) {
      delay(10);
      if (_display.displayAnimate()) {
        finished = true;
      }
    }
  }

  /**
   * The current text displayed in the display
   */
  char* _text;

  /**
   * Event triggered when an animation is done.
   */
  AnimationDoneEvent _animationDoneEvent = NULL;
};

#endif