#include <Arduino.h>
#include "LedDriverFactory.h"
#include "LedDriverSelect.h"

// -----------------------------------------------------------------------------
// Include all possible driver headers
// -----------------------------------------------------------------------------
#include "AnalogLedDriver.h"
#include "Cyw43LedDriver.h"
#include "Rgb3PinLedDriver.h"
#include "FastLedDriver.h"
#include "NullLedDriver.h"

// -----------------------------------------------------------------------------
// Instantiate a single static driver
// -----------------------------------------------------------------------------
#ifdef DRIVER_TEMPLATE
static DRIVER_TEMPLATE ledDriver;          // templated driver
#elif defined(DRIVER_ARGS_VOID)
static DRIVER_TYPE ledDriver;              // no arguments
#else
static DRIVER_TYPE ledDriver(DRIVER_ARGS); // normal driver
#endif

// -----------------------------------------------------------------------------
// Factory method
// -----------------------------------------------------------------------------
ILedDriver* LedDriverFactory::create() {
    return &ledDriver;
}
