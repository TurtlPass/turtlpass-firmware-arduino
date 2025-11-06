/*
* TTP-223: Touch Sensor
*
* Mode D: Default / No Pads Short = Default State LOW, HIGH when Touch
* Mode A: Short <A> Pads = Default State HIGH, LOW when Touch
* Mode B: Short <B> Pads = Touch to Toggle State HIGH/LOW
*
* This class handles the touch sensor events of the Mode (D)efault.
*/
#ifndef TTP_223_H
#define TTP_223_H

#include <Arduino.h>
#include "ui/LedManager.h"

#define LONG_PRESS_DURATION 500        ///< Duration in ms to detect a long press
#define SHORT_DEBOUNCE_DURATION 100    ///< Minimum duration in ms to debounce a short press

/**
 * @typedef OnSingleTouchCallback
 * @brief Callback function type invoked on a single short touch.
 */
typedef void (*OnSingleTouchCallback)();

/**
 * @typedef OnLongTouchStartCallback
 * @brief Callback function type invoked when a long touch is detected and starts.
 */
typedef void (*OnLongTouchStartCallback)();

/**
 * @typedef OnLongTouchEndCallback
 * @brief Callback function type invoked when a long touch ends (LED off or brightness 0).
 */
typedef void (*OnLongTouchEndCallback)();

/**
 * @typedef OnLongTouchCancelledCallback
 * @brief Callback function type invoked when a long touch is cancelled before completing.
 */
typedef void (*OnLongTouchCancelledCallback)();


/**
 * @class TTP223
 * @brief Handles a TTP-223 capacitive touch sensor in default mode.
 *
 * This class manages touch detection, including short touches, long touches,
 * and cancellation of long touches. Users provide callbacks for each event.
 */
class TTP223 {
public:
  /**
   * @brief Construct a TTP223 sensor handler.
   *
   * @param sensorPin GPIO pin connected to the TTP-223 sensor
   * @param singleTouch Callback invoked on single short touch
   * @param longTouchStart Callback invoked when long touch starts
   * @param longTouchEnd Callback invoked when long touch ends
   * @param longTouchCancelled Callback invoked if long touch is cancelled
   */
  TTP223(uint8_t sensorPin, OnSingleTouchCallback singleTouch, OnLongTouchStartCallback longTouchStart, OnLongTouchEndCallback longTouchEnd, OnLongTouchCancelledCallback longTouchCancelled)
    : onSingleTouch(singleTouch), onLongTouchStart(longTouchStart), onLongTouchEnd(longTouchEnd), onLongTouchCancelled(longTouchCancelled) {
    pin = sensorPin;
  }

  /**
   * @brief Initialize the sensor pin.
   *
   * Sets the sensor pin mode to INPUT.
   */
  void begin();

  /**
   * @brief Main loop handler for touch detection.
   *
   * Should be called repeatedly in the main loop. Detects short and long touches,
   * invokes appropriate callbacks, and handles long touch cancellation.
   *
   * @param brightness Current brightness level; used to detect long touch end
   */
  void loop(uint8_t brightness);

private:
  uint8_t pin;                                        ///< GPIO pin connected to the touch sensor
  OnSingleTouchCallback onSingleTouch;                ///< Callback for short touch
  OnLongTouchStartCallback onLongTouchStart;          ///< Callback for long touch start
  OnLongTouchEndCallback onLongTouchEnd;              ///< Callback for long touch end
  OnLongTouchCancelledCallback onLongTouchCancelled;  ///< Callback for cancelled long touch
  bool isTouched = false;                             ///< True if sensor currently touched
  bool singleTouchHandled = false;                    ///< True if single touch callback has been called
  bool longTouchDetected = false;                     ///< True if a long touch was detected
  bool longTouchStartHandled = false;                 ///< True if long touch start callback was called
  bool longTouchEndHandled = false;                   ///< True if long touch end callback was called
  bool longTouchCancelledHandled = false;             ///< True if long touch cancelled callback was called
  unsigned long touchStartTime = 0;                   ///< Timestamp when touch started (millis)
};

#endif  // TTP_223_H
