#ifndef BOOTSEL_BUTTON_H
#define BOOTSEL_BUTTON_H

#include <Arduino.h>
#include "LedManager.h"

#define LONG_PRESS_DURATION 500
#define SHORT_DEBOUNCE_DURATION 100

typedef void (*OnSingleTouchCallback)();
typedef void (*OnLongTouchStartCallback)();
typedef void (*OnLongTouchEndCallback)();
typedef void (*OnLongTouchCancelledCallback)();

class BootselButton {
public:
  BootselButton(OnSingleTouchCallback singleTouch, OnLongTouchStartCallback longTouchStart, OnLongTouchEndCallback longTouchEnd, OnLongTouchCancelledCallback longTouchCancelled)
    : onSingleTouch(singleTouch), onLongTouchStart(longTouchStart), onLongTouchEnd(longTouchEnd), onLongTouchCancelled(longTouchCancelled) {
  }
  void loop(uint8_t brightness);

private:
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

#endif  // BOOTSEL_BUTTON_H
