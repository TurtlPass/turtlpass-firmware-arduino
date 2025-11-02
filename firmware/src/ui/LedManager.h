#ifndef LED_MANAGER_H
#define LED_MANAGER_H

#include <FastLED.h>
#include <cmath>  // for pow()

#if defined(__TURTLPASS_LED_PIN__)
#define LED_PIN __TURTLPASS_LED_PIN__
#else
#define LED_PIN LED_BUILTIN
#endif

#if defined(__TURTLPASS_LED_IS_RGB__)
#define LED_IS_RGB __TURTLPASS_LED_IS_RGB__
#else
#define LED_IS_RGB false
#endif

#if defined(__TURTLPASS_LED_TYPE__)
#define LED_TYPE __TURTLPASS_LED_TYPE__
#else
#define LED_TYPE WS2812
#endif

#define COLOR_ORDER GRB
#define BRIGHTNESS 255
#define NUM_LEDS 1
#define NUM_COLORS 9  ///< Number of predefined colors, matches number of seed slots

/// Predefined RGB colors for LED cycling
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


/**
 * @class LedManager
 * @brief Handles LED control for single or RGB LEDs with multiple effects.
 *
 * Supports the following states:
 *  - OFF
 *  - ON
 *  - Pulsing
 *  - Blinking
 *  - Fade-out once
 *  - Fade-out loop (clock synced)
 *
 * Provides control over color, brightness, and visual effects, including
 * exponential fading.
 */
class LedManager {
public:
    /**
     * @brief Constructor. Initializes LED state and default parameters.
     */
    LedManager();

    /**
     * @brief Initializes the LED hardware and FastLED library.
     */
    void begin();

    /**
     * @brief Updates the LED state and applies visual effects.
     * Should be called repeatedly in the main loop.
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
    uint8_t getColorIndex();

    /**
     * @brief Get the current LED brightness.
     * @return Current brightness (0..255).
     */
    uint8_t getCurrentBrightness();

    /**
     * @brief Compute and get new brightness based on current state/effect.
     * @return Computed brightness (0..255).
     */
    uint8_t getNewBrightness();

private:
    CRGB leds[NUM_LEDS];  ///< Array of LEDs (single or RGB)

    /// Internal LED states
    enum State {
        LED_OFF = 0,          ///< LED is off
        LED_ON = 1,           ///< LED is on at full brightness
        LED_PULSING = 2,      ///< LED brightness oscillates
        LED_BLINKING = 3,     ///< LED toggles on/off
        LED_FADE_OUT_ONCE = 4,///< LED fades out once
        LED_FADE_OUT_LOOP = 5 ///< LED fades out repeatedly
    };

    /// Struct for managing LED parameters and state
    struct Led {
        uint8_t colorIndex;             ///< Current color index
        uint8_t brightness;             ///< Current brightness (0..255)
        int fadeAmount;                 ///< Fade increment/decrement for pulsing/fade
        int blinkState;                 ///< Current blink state (on/off)
        int blinkSpeed;                 ///< Blink update interval in ms
        int pulseSpeed;                 ///< Pulse update interval in ms
        unsigned long lastUpdate;       ///< Timestamp of last update
        unsigned long fadeOutStartTime; ///< Timestamp when fade out started
        unsigned long fadeOutDuration;  ///< Fade out duration in ms
        State state;                    ///< Current LED state/effect
    };

    Led led;  ///< Instance of LED struct

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
};

#endif  // LED_MANAGER_H
