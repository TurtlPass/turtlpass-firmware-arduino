#pragma once
#include <Arduino.h>

// Helper macros
#define STR_HELPER(x) #x
#define STRIFY(x) STR_HELPER(x)


// -----------------------------------------------------------------------------
// LED driver selection logic (board/environment dependent)
// -----------------------------------------------------------------------------
#pragma message("LED DRIVER FACTORY: Detecting board...")

// Raspberry Pi Pico or Pico 2 (simple analog LED)
#if defined(ARDUINO_RASPBERRY_PI_PICO) || defined(ARDUINO_RASPBERRY_PI_PICO_2)
  #define DRIVER_TYPE AnalogLedDriver
  #define DRIVER_ARGS LED_BUILTIN
  #pragma message("  -> Analog (mono LED) driver selected")

// CYW43 WiFi chip LED (e.g. Pico W, Pico W2, etc.)
#elif defined(PICO_CYW43_SUPPORTED)
  #define DRIVER_TYPE Cyw43LedDriver
  #define DRIVER_ARGS_VOID
  #pragma message("  -> CYW43 LED driver selected")

// RGB 3-pin LED (e.g. Arduino Nano RP2040 Connect, Pimoroni Tiny2040, Pimoroni Tiny2350)
#elif defined(TP_RGB_3PIN_LED) && defined(TP_PIN_LED_R) && defined(TP_PIN_LED_G) && defined(TP_PIN_LED_B)
  #define DRIVER_TYPE Rgb3PinLedDriver
  #define DRIVER_ARGS TP_PIN_LED_R, TP_PIN_LED_G, TP_PIN_LED_B
  #pragma message("  -> RGB 3Pin LED driver selected")

// RGB LED (single-pin addressable)
#elif defined(TP_RGB_LED) && (TP_RGB_LED) && defined(TP_PIN_LED)
  #define DRIVER_TYPE FastLedDriver
  #define DRIVER_TEMPLATE FastLedDriver<TP_PIN_LED>
  #pragma message("  -> FastLED driver selected, pin=" STRIFY(TP_PIN_LED))

// Analog LED (single-pin, not RGB)
#elif defined(TP_RGB_LED) && (!TP_RGB_LED) && defined(TP_PIN_LED)
  #define DRIVER_TYPE AnalogLedDriver
  #define DRIVER_ARGS TP_PIN_LED
  #pragma message("  -> Analog (mono LED) driver selected, pin=" STRIFY(TP_PIN_LED))

// Fallback RGB LED using LED_BUILTIN
#elif defined(TP_RGB_LED) && (TP_RGB_LED) && defined(LED_BUILTIN)
  #define DRIVER_TYPE FastLedDriver
  #define DRIVER_TEMPLATE FastLedDriver<LED_BUILTIN>
  #pragma message("  -> FastLED driver selected, pin=" STRIFY(LED_BUILTIN))

// Fallback Analog LED using LED_BUILTIN
#elif defined(TP_RGB_LED) && (!TP_RGB_LED) && defined(LED_BUILTIN)
  #define DRIVER_TYPE AnalogLedDriver
  #define DRIVER_ARGS LED_BUILTIN
  #pragma message("  -> Analog (mono LED) driver selected, pin=" STRIFY(LED_BUILTIN))

// Null driver
#else
  #define DRIVER_TYPE NullLedDriver
  #define DRIVER_ARGS_VOID
  #pragma message("  -> No supported LED driver found for this board!")
#endif
