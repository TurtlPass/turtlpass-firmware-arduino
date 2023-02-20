#ifndef LED_MANAGER_H
#define LED_MANAGER_H

#include <Arduino.h>
#include <FastLED.h>

#if defined(__TURTLPASS_LED_PIN__)
#define LED_PIN __TURTLPASS_LED_PIN__
#else
#define LED_PIN 16
#endif

#if defined(__TURTLPASS_LED_IS_RGB__)
#define LED_IS_RGB __TURTLPASS_LED_IS_RGB__
#else
#define LED_IS_RGB true
#endif

#if defined(__TURTLPASS_LED_TYPE__)
#define LED_TYPE __TURTLPASS_LED_TYPE__
#else
#define LED_TYPE WS2812
#endif

#define COLOR_ORDER GRB
#define BRIGHTNESS 255
#define NUM_LEDS 1
#define NUM_COLORS 9  // matches the number of seeds

const CRGB colors[NUM_COLORS] = {
  CRGB::Green,
  CRGB::Yellow,
  CRGB::Red,
  CRGB::Blue,
  CRGB::White,
  CRGB::BlueViolet,
  CRGB::OrangeRed,
  CRGB::Aqua,
  CRGB::DeepPink,
};

#define DEFAULT_FADE_AMOUNT 3
#define DEFAULT_BRIGHTNESS 255
#define MAX_LONG 1410065407
#define LED_UPDATES_PER_SECOND 100


class LedManager {
public:
  LedManager() {
    led.brightness = DEFAULT_BRIGHTNESS;
    led.fadeAmount = DEFAULT_FADE_AMOUNT;  // step
    led.pulseSpeed = 6;                    // in ms
    led.blinkSpeed = 60;                   // in ms
    led.blinkState = LOW;
    led.lastUpdate = MAX_LONG;
    led.state = LED_ON;
  }
  void setup();
  void loop();
  void setOff();
  void setOn();
  void setPulsing();
  void setBlinking();
  void setFadeOutOnce(uint32_t intervalInSeconds);
  void setFadeOutLoop(uint32_t intervalInSeconds, uint32_t currentTime);
  void showNextColor();
  void setColorIndex(uint8_t colorIndex);
  uint8_t getColorIndex();
  uint8_t getCurrentBrightness();
  uint8_t getNewBrightness();

private:
  CRGB leds[NUM_LEDS];
  enum State {
    LED_OFF = 0,
    LED_ON = 1,
    LED_PULSING = 2,
    LED_BLINKING = 3,
    LED_FADE_OUT_ONCE = 4,
    LED_FADE_OUT_LOOP = 5,
  };
  struct Led {
    uint8_t colorIndex;
    uint8_t brightness;
    int fadeAmount;
    int blinkState;
    int blinkSpeed;
    int pulseSpeed;
    unsigned long lastUpdate;
    unsigned long fadeOutStartTime;
    unsigned long fadeOutDuration;
    State state;
  };
  Led led;
  int mapExponentially(float value, float inMin, float inMax, float outMin, float outMax);
};

#endif  // LED_MANAGER_H
