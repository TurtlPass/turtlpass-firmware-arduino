#ifndef TOUCH_HANDLER_H
#define TOUCH_HANDLER_H

#include "InternalState.h"
#include "ui/LedManager.h"
#include "core/CommandProcessor.h"


/**
 * @brief Handles touch input events from the TTP223 or Bootsel button.
 * 
 * This class centralizes the logic for single and long touches, including:
 * - Switching LED colors
 * - Starting and ending long touches
 * - Triggering password typing via CommandProcessor
 */
class TouchHandler {
public:
    /**
     * @brief Constructs a TouchHandler instance.
     * 
     * @param state Reference to the shared InternalState variable.
     * @param led Reference to the LedManager instance for LED feedback.
     * @param cmdProcessor Reference to the CommandProcessor for password derivation and command handling.
     */
    TouchHandler(InternalState &state, LedManager &led, CommandProcessor &cmdProcessor);

    /**
     * @brief Handles a single touch event.
     * 
     * Behavior depends on the current internal state:
     * - IDLE: cycles to the next LED color
     * - PASSWORD_READY: triggers typing the password
     * - Other states: ignored
     */
    void onSingleTouch();

    /**
     * @brief Handles the start of a long touch event.
     * 
     * Typically transitions the state from IDLE to TOUCHING and updates LEDs.
     */
    void onLongTouchStart();

    /**
     * @brief Handles the end of a long touch event.
     * 
     * Typically triggers default password derivation using CommandProcessor
     * and HID typing if successful. Updates internal state back to IDLE.
     */
    void onLongTouchEnd();

    /**
     * @brief Handles cancellation of a long touch event.
     * 
     * Resets internal state to IDLE and restores LED status.
     */
    void onLongTouchCancelled();
    
    /**
     * @brief Types the currently derived password via HID.
     * 
     * This method transitions the internal state to `TYPING`, provides LED feedback
     * during the typing process, and sends the password string through the HID interface.
     * Once typing is complete, it clears the output buffer and restores the LED to steady on.
     * 
     * Typical sequence:
     * 1. Set internal state to `TYPING`
     * 2. Blink LED to indicate typing activity
     * 3. Send password from CommandProcessor output buffer via HID
     * 4. Clear output buffer and delay briefly
     * 5. Turn LED solid on to signal completion
     */
    void typePassword();

private:
    InternalState &internalState_;       /**< Reference to the shared internal state */
    LedManager &ledManager_;             /**< Reference to the LED manager */
    CommandProcessor &commandProcessor_; /**< Reference to the command processor */
};

#endif
