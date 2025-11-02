/*
* TTP-223: Touch Sensor
*
* Mode D: Default / No Pads Short = Default State LOW, HIGH when Touch
* Mode A: Short <A> Pads = Default State HIGH, LOW when Touch
* Mode B: Short <B> Pads = Touch to Toggle State HIGH/LOW
*
* This class handles the touch sensor events of the Mode (D)efault.
*/
#include "input/TTP223.h"

void TTP223::begin() {
  pinMode(pin, INPUT);
}

void TTP223::loop(uint8_t brightness) {
  if (digitalRead(pin)) {
    longTouchCancelledHandled = false;
    if (!longTouchEndHandled && brightness == 0) {
      onLongTouchEnd();
      longTouchEndHandled = true;
    }
    if (!isTouched) {
      isTouched = true;
      touchStartTime = millis();
      longTouchDetected = false;

      while (digitalRead(pin)) {
        if ((millis() - touchStartTime) > LONG_PRESS_DURATION) {
          longTouchDetected = true;
          break;
        }
        delay(10);
      }
      if (longTouchDetected && !longTouchStartHandled) {
        onLongTouchStart();
        longTouchStartHandled = true;
        singleTouchHandled = true;

      } else if (!longTouchDetected && !singleTouchHandled) {
        onSingleTouch();
        singleTouchHandled = true;
        delay(100);  // delay to prevent repeated calls
      }
    }
  } else {
    // when the touch is released
    if (longTouchDetected && !longTouchCancelledHandled && !longTouchEndHandled && longTouchStartHandled) {
      onLongTouchCancelled();
      longTouchCancelledHandled = true;
    }
    // reset flags
    isTouched = false;
    singleTouchHandled = false;
    longTouchStartHandled = false;
    longTouchEndHandled = false;
  }
  delay(10);
}
