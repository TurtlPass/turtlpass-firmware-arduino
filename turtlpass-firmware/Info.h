#ifndef INFO_H
#define INFO_H

#include <Arduino.h>
#include "pico/unique_id.h"

#if defined(__TURTLPASS_VERSION__)
#define TURTLPASS_VERSION __TURTLPASS_VERSION__
#else
#define TURTLPASS_VERSION "development"
#endif

void deviceInfo();

#endif // INFO_H
