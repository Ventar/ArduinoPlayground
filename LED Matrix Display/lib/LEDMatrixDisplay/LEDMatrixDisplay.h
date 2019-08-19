#ifndef LEDMatrixDisplay_h
#define LEDMatrixDisplay_h

#include <Arduino.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <SPI.h>
#include <vector>

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
   * Animation object used to queue multiple animantions for a single text. Can be used to have different animation for
   *showing a text for the first time (e.g. select a menu entry) and showing it in a continues loop.
   */
  struct Animation {
    Animation(textPosition_t align, textEffect_t effectIn, textEffect_t effectOut, uint16_t speed = 30,
              uint16_t pause = 1000) {
      this->align = align;
      this->effectIn = effectIn;
      this->effectOut = effectOut;
      this->speed = speed;
      this->pause = pause;
    }

    /**
     * The alignment of the text within the display.
     */
    textPosition_t align;
    /**
     * Controls the delay between the individual transuition steps of an animation effect.
     */
    uint16_t speed;
    /**
     * Defines the pause in ms the display should take into account before switching to the out effect.
     */
    uint16_t pause;
    /**
     * The text effect to show the text.
     */
    textEffect_t effectIn;

    /**
     * The text effect to hide the text.
     */
    textEffect_t effectOut;
  };

  /**
   * Callback triggered when the animation is done.
   */
  typedef std::function<void()> AnimationDoneEvent;

  /**
   * Callback triggered to execute program code between the in and out effect of
   * the display.
   */
  typedef std::function<void()> SyncTextCodeBlockCallback;

 public:
  MD_Parola _display = MD_Parola(MD_MAX72XX::FC16_HW, DATA_PIN, CLK_PIN, CS_PIN, 4);

  /**
   * Utitlity function to show a setup text while the device is starting. The Text is shown with the given effect and
   * the display is cleared when the business logic in the callback was executed. During this time the display will
   * block the execution of all other code, i.e. you should not use this within the loop methods of your code unless you
   * do not need to react on other events.
   *
   * @param text the text to display
   * @param codeCallback the code to execute between displaying the text and fading it out
   * @param the alignment of the text within the display.
   * @param effectIn the text effect that should be used to display the text
   * @param effectOut the text effect that should be used to remove the text
   * @param speed Controls the delay between the individual transuition steps of an animation effect.
   * @param pause defines the pause in ms the display should take into account before switching to the out effect.
   */
  void displayTextSync(String text, SyncTextCodeBlockCallback codeCallback, textPosition_t align = PA_CENTER,
                       textEffect_t effectIn = PA_SCROLL_DOWN, textEffect_t effectOut = PA_SCROLL_UP,
                       uint16_t speed = 30, uint16_t pause = 1000) {
    delete[] _text;
    _text = new char[text.length() + 1];
    text.toCharArray(_text, text.length() + 1);

    this->_loop = false;

    delete[] _initAnimation;
    delete[] _showAnimation;

    _initAnimation = new Animation(align, effectIn, PA_NO_EFFECT, speed, pause);
    this->waitForAnimation();
    delete[] _initAnimation;

    codeCallback();

    _initAnimation = new Animation(align, PA_PRINT, effectOut, speed, pause);
    this->waitForAnimation();
    // delete[] _initAnimation;
  }

  /**
   * Utitlity function to initialize a text on the display with an animation and shop a second animation afterwards. The
   * second animation can be run in a loop to continous show the text. This is useful in case the text itself does not
   * fit the display.
   *
   * @param text the text to display
   * @param effectIn the text effect that should be used to display the text
   */
  void displayInitAndShow(String text, textPosition_t align, textEffect_t effectIn, textEffect_t effectShow,
                          textEffect_t effectOut, bool loop, uint16_t speed = 30, uint16_t pause = 1000) {
    delete[] _text;
    _text = new char[text.length() + 1];
    text.toCharArray(_text, text.length() + 1);

    this->_loop = loop;

    delete[] _initAnimation;
    delete[] _showAnimation;
    _initAnimation = new Animation(align, effectIn, PA_NO_EFFECT, speed, pause);
    _showAnimation = new Animation(align, effectShow, effectOut, speed, pause);

    _display.displayText(_text, _initAnimation->align, _initAnimation->speed, _initAnimation->pause,
                         _initAnimation->effectIn, _initAnimation->effectOut);
  }

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
    /*
    if (_initAnimation == NULL && _showAnimation != NULL) {
      _display.displayText(_text, _showAnimation->align, _showAnimation->speed, _showAnimation->pause,
                           _showAnimation->effectIn, _showAnimation->effectOut);
    }
    */

    if (_display.displayAnimate()) {
      if (_initAnimation != NULL && _showAnimation != NULL) {
        _display.displayText(_text, _showAnimation->align, _showAnimation->speed, _showAnimation->pause,
                             _showAnimation->effectIn, _showAnimation->effectOut);
      }

      /*
            if (_loop) {
              _display.displayReset();
            }

            if (_animationDoneEvent) {
              _animationDoneEvent();
              _animationDoneEvent = NULL;
            }
            */
    }
  }

  /**
   * Sets the text that is displayed on the display.
   */
  void setTextWithCallback(char *text, textPosition_t align, textEffect_t effectIn, textEffect_t effectOut,
                           AnimationDoneEvent doneEvent = NULL, uint16_t speed = 30, uint16_t pause = 1000) {
    _animationDoneEvent = doneEvent;
    _loop = false;
    setText(text, align, effectIn, effectOut, false, false, false, speed, pause);
  }

  /**
   * Sets the text that is displayed on the display.
   */
  void setText(String text, textPosition_t align, textEffect_t effectIn, textEffect_t effectOut, bool loop = false,
               bool waitForAnimation = false, bool clear = false, uint16_t speed = 30, uint16_t pause = 1000) {
    _loop = loop;
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
  /**
   * Animated the display in a syncronous way, i.e. all animation steps are executed before the method returns. During
   * this time the display will block the execution of all other code, i.e. you should not use this within the loop
   * methods of your code unless you do not need to react on other events
   */
  void waitForAnimation() {
    bool finished = false;
    _display.displayText(_text, _initAnimation->align, _initAnimation->speed, _initAnimation->pause,
                         _initAnimation->effectIn, _initAnimation->effectOut);
    while (!finished) {
      delay(10);
      if (_display.displayAnimate()) {
        finished = true;
      }
    }
  }

  /**s
   * The current text displayed in the display
   */
  char *_text;

  /**
   * If the animation is shown in an endles loop.
   */
  bool _loop;

  Animation *_initAnimation;
  Animation *_showAnimation;

  /**
   * Event triggered when an animation is done.
   */
  AnimationDoneEvent _animationDoneEvent = NULL;
};

#endif