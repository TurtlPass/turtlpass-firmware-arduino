#pragma once

#include <stdint.h>
#include "ui/driver/ILedDriver.h"

#define LED_UPDATES_PER_SECOND 100
#define FADE_AMOUNT 3
#define MAX_BRIGHTNESS 255
#define MAX_LONG 1410065407


/**
 * @class LedManager
 * @brief High-level controller for LED effects and animations.
 *
 * Supports the following states:
 *  - OFF
 *  - ON
 *  - Pulsing
 *  - Blinking
 *  - Fade-out once
 *  - Fade-out loop (clock synced)
 *
 * The LedManager provides platform-independent control over color, 
 * brightness, and visual effects, including exponential fading.
 * 
 * It delegates the actual LED control to a hardware-specific
 * implementation of the ILedDriver interface (e.g., FastLED, CYW43, etc).
 */
class LedManager {
public:
    /**
     * @brief Construct a new LedManager.
     * @param driver Pointer to a hardware-specific LED driver implementation.
     *
     * The driver is not owned by LedManager; it must remain valid
     * for the lifetime of this object.
     */
    explicit LedManager(ILedDriver* driver);

    /**
     * @brief Initializes the LED hardware and resets animation state.
     */
    void begin();

    /**
     * @brief Periodically updates LED brightness and effects.
     *
     * This should be called regularly inside the main loop to maintain
     * smooth animations (e.g., every 10–20 ms).
     */
    void loop();

    /**
     * @brief Turn LED off immediately.
     */
    void setOff();

    /**
     * @brief Turn LED on at full brightness.
     */
    void setOn();

    /**
     * @brief Start pulsing effect (brightness oscillates).
     */
    void setPulsing();

    /**
     * @brief Start blinking effect (LED toggles on/off).
     */
    void setBlinking();

    /**
     * @brief Start a one-time fade out effect.
     * @param intervalInSeconds Duration of the fade out in seconds.
     */
    void setFadeOutOnce(uint32_t intervalInSeconds);

    /**
     * @brief Start a looping fade out effect, synchronized to a clock.
     * @param intervalInSeconds Duration of each fade-out loop in seconds.
     * @param currentTime Current timestamp (in milliseconds) to synchronize the loop.
     */
    void setFadeOutLoop(uint32_t intervalInSeconds, uint32_t currentTime);

    /**
     * @brief Cycle to the next predefined color.
     */
    void showNextColor();

    /**
     * @brief Set the current color index.
     * @param colorIndex Index into the colors array (0..NUM_COLORS-1).
     */
    void setColorIndex(uint8_t colorIndex);

    /**
     * @brief Get the current color index.
     * @return Current color index (0..NUM_COLORS-1).
     */
    uint8_t getColorIndex() const;

    /**
     * @brief Get the current LED brightness.
     * @return Current brightness (0..255).
     */
    uint8_t getCurrentBrightness() const;

private:
    /**
     * @enum State
     * @brief Represents internal animation states for the LED.
     */
    enum State {
        LED_OFF = 0,          ///< LED is off
        LED_ON = 1,           ///< LED is on at full brightness
        LED_PULSING = 2,      ///< LED brightness oscillates
        LED_BLINKING = 3,     ///< LED toggles on/off
        LED_FADE_OUT_ONCE = 4,///< LED fades out once
        LED_FADE_OUT_LOOP = 5 ///< LED fades out repeatedly
    };

    /**
     * @struct Led
     * @brief Internal structure holding LED parameters and state.
     */
    struct Led {
        uint8_t colorIndex = 0;                 ///< Current color index
        uint8_t brightness = 255;               ///< Current brightness (0..255)
        int fadeAmount = 3;                     ///< Fade increment/decrement for pulsing/fade
        int blinkState = 0;                     ///< Current blink state (on/off)
        int blinkSpeed = 60;                    ///< Blink update interval in ms
        int pulseSpeed = 6;                     ///< Pulse update interval in ms
        unsigned long lastUpdate = MAX_LONG;    ///< Timestamp of last update
        unsigned long fadeOutStartTime = 0;     ///< Timestamp when fade out started
        unsigned long fadeOutDuration = 0;      ///< Fade out duration in ms
        State state = LED_ON;                   ///< Current LED state/effect
    } led;

    ILedDriver* driver;         ///< Hardware-specific LED driver instance.
    uint32_t lastLoopTime = 0;  ///< Timestamp of the last main loop update.

    /**
     * @brief Compute the next brightness value based on current animation.
     * @return The new brightness (0–255).
     */
    uint8_t getNewBrightness();

    /**
     * @brief Maps a value from one range to another using an exponential curve.
     *
     * Useful for non-linear fading (fast at start, slower near zero).
     * @param value Input value to map.
     * @param inMin Minimum of input range.
     * @param inMax Maximum of input range.
     * @param outMin Minimum of output range.
     * @param outMax Maximum of output range.
     * @return Mapped value.
     */
    int mapExponentially(float value, float inMin, float inMax, float outMin, float outMax);

    static const uint8_t NUM_COLORS = 9;        ///< Number of predefined color presets.
    static const uint8_t colors[NUM_COLORS][3]; ///< RGB lookup table for color presets.
};
