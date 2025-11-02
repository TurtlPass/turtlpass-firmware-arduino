#ifndef BOOTSEL_BUTTON_H
#define BOOTSEL_BUTTON_H

#include "ui/LedManager.h"

#define LONG_PRESS_DURATION 500        ///< Duration in ms to detect a long press
#define SHORT_DEBOUNCE_DURATION 100    ///< Minimum duration in ms to debounce a short press

/**
 * @typedef OnSingleTouchCallback
 * @brief Callback function type invoked on a single short press.
 */
typedef void (*OnSingleTouchCallback)();

/**
 * @typedef OnLongTouchStartCallback
 * @brief Callback function type invoked when a long press is detected and starts.
 */
typedef void (*OnLongTouchStartCallback)();

/**
 * @typedef OnLongTouchEndCallback
 * @brief Callback function type invoked when a long press ends (brightness 0 or release).
 */
typedef void (*OnLongTouchEndCallback)();

/**
 * @typedef OnLongTouchCancelledCallback
 * @brief Callback function type invoked when a long press is cancelled before completing.
 */
typedef void (*OnLongTouchCancelledCallback)();


/**
 * @class BootselButton
 * @brief Handles a BOOTSEL hardware button with short and long press detection.
 *
 * This class detects short presses, long presses, and cancellation of long presses
 * on the BOOTSEL button. Users provide callbacks for each type of event.
 */
class BootselButton {
public:

  /**
   * @brief Construct a BootselButton handler.
   *
   * @param singleTouch Callback invoked on single short press
   * @param longTouchStart Callback invoked when long press starts
   * @param longTouchEnd Callback invoked when long press ends
   * @param longTouchCancelled Callback invoked if long press is cancelled
   */
  BootselButton(OnSingleTouchCallback singleTouch, OnLongTouchStartCallback longTouchStart, OnLongTouchEndCallback longTouchEnd, OnLongTouchCancelledCallback longTouchCancelled)
    : onSingleTouch(singleTouch), onLongTouchStart(longTouchStart), onLongTouchEnd(longTouchEnd), onLongTouchCancelled(longTouchCancelled) {
  }

  /**
   * @brief Main loop handler for button detection.
   *
   * Should be called repeatedly in the main loop. Detects short and long presses,
   * invokes appropriate callbacks, and handles long press cancellation.
   *
   * @param brightness Current brightness level; used to detect long press end
   */
  void loop(uint8_t brightness);

private:
    OnSingleTouchCallback onSingleTouch;                ///< Callback for short press
    OnLongTouchStartCallback onLongTouchStart;          ///< Callback for long press start
    OnLongTouchEndCallback onLongTouchEnd;              ///< Callback for long press end
    OnLongTouchCancelledCallback onLongTouchCancelled;  ///< Callback for cancelled long press
    bool isTouched = false;                             ///< True if button currently pressed
    bool singleTouchHandled = false;                    ///< True if short press callback has been called
    bool longTouchDetected = false;                     ///< True if a long press was detected
    bool longTouchStartHandled = false;                 ///< True if long press start callback was called
    bool longTouchEndHandled = false;                   ///< True if long press end callback was called
    bool longTouchCancelledHandled = false;             ///< True if long press cancelled callback was called
    unsigned long touchStartTime = 0;                   ///< Timestamp when press started (millis)
};

#endif  // BOOTSEL_BUTTON_H
