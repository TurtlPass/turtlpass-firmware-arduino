#ifndef PASSWORD_TYPER_H
#define PASSWORD_TYPER_H

#include "ui/LedManager.h"
#include <cstdint>


/**
 * @brief Handles typing passwords via HID keyboard and LED feedback.
 * 
 * This class centralizes the logic for sending a password buffer
 * to the HID keyboard while managing LED states such as blinking
 * and pulsing.
 */
class PasswordTyper {
public:
    /**
     * @brief Constructs a PasswordTyper instance.
     * 
     * @param led Reference to the LedManager for visual feedback.
     * @param buffer Pointer to the password buffer to type.
     * @param size Size of the password buffer.
     */
    PasswordTyper(LedManager &led, uint8_t *buffer, size_t size);

    /**
     * @brief Types the current password buffer via HID keyboard.
     * 
     * Steps performed:
     * 1. Sets LEDs to blinking.
     * 2. Sends the password via HID.
     * 3. Clears the buffer.
     * 4. Restores LEDs to normal state.
     */
    void type();

private:
    LedManager &ledManager_; /**< Reference to the LED manager */
    uint8_t *buffer_;        /**< Pointer to the password buffer */
    size_t bufferSize_;      /**< Size of the password buffer */
};

#endif
