#pragma once
#include <cstdint>
#include <cstdio>
#include <chrono>
#include <thread>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t millis(void);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

// Global fake time
inline uint32_t& fakeMillisTime() {
    static uint32_t t = 0;
    return t;
}

// C-linkage millis returns current fake time
inline uint32_t millis(void) {
    return fakeMillisTime();
}

// Advance time manually
inline void advanceMillis(uint32_t ms) {
    fakeMillisTime() += ms;
}

// Mock Arduino delay
inline void delay(uint32_t ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    advanceMillis(ms); // also advance fake time
}

// Other Arduino mocks
inline void pinMode(int, int) {}
inline void digitalWrite(int, int) {}
inline int digitalRead(int) { return 0; }

typedef uint8_t byte;
typedef uint8_t boolean;
#ifndef HIGH
#define HIGH 0x1
#endif
#ifndef LOW
#define LOW 0x0
#endif

#endif
