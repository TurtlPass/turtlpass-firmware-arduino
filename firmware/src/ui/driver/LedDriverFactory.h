#pragma once
#include "ui/driver/ILedDriver.h"

/**
 * @class LedDriverFactory
 * @brief Factory responsible for creating the correct LED driver based on board macros.
 */
class LedDriverFactory {
public:
    /**
     * @brief Create and return a pointer to the appropriate LED driver instance.
     */
    static ILedDriver* create();
};
