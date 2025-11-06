#include <unity.h>
#include <cstdint>
#include <vector>
#include <cmath>
#include <chrono>
#include <thread>

// -----------------------------------------------------------------------------
// Include LedManager with private access for testing
// -----------------------------------------------------------------------------
#define private public
#define protected public
#include "ui/LedManager.h"
#include "ui/LedManager.cpp"
#undef private
#undef protected

// -----------------------------------------------------------------------------
// Mock LED driver
// -----------------------------------------------------------------------------
struct MockLedDriver : public ILedDriver {
    uint8_t lastR = 0, lastG = 0, lastB = 0;
    uint8_t lastBrightness = 0;
    bool beginCalled = false;
    void begin() override { beginCalled = true; }
    void setColor(uint8_t r, uint8_t g, uint8_t b) override { lastR = r; lastG = g; lastB = b; }
    void setBrightness(uint8_t b) override { lastBrightness = b; }
    void show() override {}
};

// -----------------------------------------------------------------------------
// Tests
// -----------------------------------------------------------------------------
void test_initialization_and_color_index() {
    MockLedDriver driver;
    LedManager lm(&driver);

    lm.begin();
    TEST_ASSERT_TRUE(driver.beginCalled);

    TEST_ASSERT_EQUAL_UINT8(0, lm.getColorIndex());

    for (uint8_t i = 0; i < 9; ++i) lm.showNextColor();
    TEST_ASSERT_EQUAL_UINT8(0, lm.getColorIndex());

    lm.setColorIndex(5);
    TEST_ASSERT_EQUAL_UINT8(5, lm.getColorIndex());
}

void test_on_off_brightness() {
    MockLedDriver driver;
    LedManager lm(&driver);

    lm.setOn();
    TEST_ASSERT_EQUAL_UINT8(MAX_BRIGHTNESS, lm.getCurrentBrightness());

    lm.setOff();
    TEST_ASSERT_EQUAL_UINT8(0, lm.getCurrentBrightness());
}

void test_pulsing_behavior() {
    MockLedDriver driver;
    LedManager lm(&driver);

    lm.setPulsing();
    uint8_t first = lm.getNewBrightness();
    delay(500);
    uint8_t second = lm.getNewBrightness();

    TEST_ASSERT_TRUE(first <= MAX_BRIGHTNESS);
    TEST_ASSERT_TRUE(second <= MAX_BRIGHTNESS);
}

void test_blinking_behavior() {
    MockLedDriver driver;
    LedManager lm(&driver);

    lm.setBlinking();
    uint8_t first = lm.getNewBrightness();
    delay(200);
    uint8_t second = lm.getNewBrightness();

    TEST_ASSERT_TRUE(first == 0 || first == MAX_BRIGHTNESS);
    TEST_ASSERT_TRUE(second == 0 || second == MAX_BRIGHTNESS);
}

void test_mapExponentially() {
    MockLedDriver driver;
    LedManager lm(&driver);

    int val1 = lm.mapExponentially(0, 0, 1000, 0, 255);
    int val2 = lm.mapExponentially(500, 0, 1000, 0, 255);
    int val3 = lm.mapExponentially(1000, 0, 1000, 0, 255);

    TEST_ASSERT_EQUAL_INT(0, val1);
    TEST_ASSERT_TRUE(val2 > 0 && val2 < 255);
    TEST_ASSERT_EQUAL_INT(255, val3);
}

void test_fade_out_once() {
    MockLedDriver driver;
    LedManager lm(&driver);

    lm.setFadeOutOnce(1);

    TEST_ASSERT_EQUAL_UINT8(MAX_BRIGHTNESS, lm.getCurrentBrightness());

    advanceMillis(200);
    uint8_t brightness = lm.getNewBrightness();
    TEST_ASSERT_TRUE(brightness < MAX_BRIGHTNESS);
    TEST_ASSERT_TRUE(brightness > 0);

    advanceMillis(400);
    brightness = lm.getNewBrightness();
    TEST_ASSERT_TRUE(brightness < MAX_BRIGHTNESS);
    TEST_ASSERT_TRUE(brightness > 0);

    advanceMillis(400);
    brightness = lm.getNewBrightness();
    TEST_ASSERT_EQUAL_UINT8(0, brightness);
}

void test_fade_out_loop() {
    MockLedDriver driver;
    LedManager lm(&driver);

    lm.setFadeOutLoop(1, 0);

    uint8_t b1 = lm.getNewBrightness();
    advanceMillis(500);
    uint8_t b2 = lm.getNewBrightness();
    advanceMillis(500);
    uint8_t b3 = lm.getNewBrightness();

    TEST_ASSERT_NOT_EQUAL(b1, b2);
    TEST_ASSERT_NOT_EQUAL(b2, b3);
}

void test_driver_output() {
    MockLedDriver driver;
    LedManager lm(&driver);

    lm.setColorIndex(2);
    lm.setOn();
    lm.loop();

    TEST_ASSERT_EQUAL_UINT8(255, driver.lastR);
    TEST_ASSERT_EQUAL_UINT8(0, driver.lastG);
    TEST_ASSERT_EQUAL_UINT8(0, driver.lastB);
    TEST_ASSERT_EQUAL_UINT8(MAX_BRIGHTNESS, driver.lastBrightness);
}

// -----------------------------------------------------------------------------
// Test Runner
// -----------------------------------------------------------------------------
int main() {
    UNITY_BEGIN();
    RUN_TEST(test_initialization_and_color_index);
    RUN_TEST(test_on_off_brightness);
    RUN_TEST(test_pulsing_behavior);
    RUN_TEST(test_blinking_behavior);
    RUN_TEST(test_mapExponentially);
    RUN_TEST(test_fade_out_once);
    RUN_TEST(test_fade_out_loop);
    RUN_TEST(test_driver_output);
    return UNITY_END();
}
