#include "BootselButton.h"

void BootselButton::loop(uint8_t brightness) {
  if (BOOTSEL) {
    longTouchCancelledHandled = false;
    if (!longTouchEndHandled && brightness == 0) {
      onLongTouchEnd();
      longTouchEndHandled = true;
    }
    if (!isTouched) {
      isTouched = true;
      touchStartTime = millis();
      longTouchDetected = false;

      while (BOOTSEL) {
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
