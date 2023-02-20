/*
* TTP-223: Touch Sensor
*
* Mode D: Default / No Pads Short = Default State LOW, HIGH when Touch
* Mode A: Short <A> Pads = Default State HIGH, LOW when Touch
* Mode B: Short <B> Pads = Touch to Toggle State HIGH/LOW
*
* This class handles the touch sensor events of the Mode (D)efault.
*/
#ifndef TTP223_H
#define TTP223_H

#include "LedManager.h"

#if defined(__TURTLPASS_PIN_TTP223__)
#define PIN_TTP223_SENSOR __TURTLPASS_PIN_TTP223__
#else
#define PIN_TTP223_SENSOR 2
#endif

#define LONG_PRESS_DURATION 500
#define SHORT_DEBOUNCE_DURATION 100

typedef void (*OnSingleTouchCallback)();
typedef void (*OnLongTouchStartCallback)();
typedef void (*OnLongTouchEndCallback)();
typedef void (*OnLongTouchCancelledCallback)();

class TTP223 {
public:
  TTP223(uint8_t sensorPin, OnSingleTouchCallback singleTouch, OnLongTouchStartCallback longTouchStart, OnLongTouchEndCallback longTouchEnd, OnLongTouchCancelledCallback longTouchCancelled)
    : onSingleTouch(singleTouch), onLongTouchStart(longTouchStart), onLongTouchEnd(longTouchEnd), onLongTouchCancelled(longTouchCancelled) {
    pin = sensorPin;
  }
  void begin();
  void loop(uint8_t brightness);

private:
  uint8_t pin;
  OnSingleTouchCallback onSingleTouch;
  OnLongTouchStartCallback onLongTouchStart;
  OnLongTouchEndCallback onLongTouchEnd;
  OnLongTouchCancelledCallback onLongTouchCancelled;
  bool isTouched = false;
  bool singleTouchHandled = false;
  bool longTouchDetected = false;
  bool longTouchStartHandled = false;
  bool longTouchEndHandled = false;
  bool longTouchCancelledHandled = false;
  unsigned long touchStartTime = 0;
};

#endif  // TTP223_H
