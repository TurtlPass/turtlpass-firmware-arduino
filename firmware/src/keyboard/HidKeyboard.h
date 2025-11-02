#pragma once
#include "Adafruit_TinyUSB.h"
#include "tusb.h"

// Initialize TinyUSB keyboard interface
void hidKeyboardInit();

// Send a single key (ASCII-aware)
void hidSendKey(char c);

// Type a full string (ASCII-aware)
void hidTypeString(const char* str);

// Send Enter key
void hidSendEnter();
