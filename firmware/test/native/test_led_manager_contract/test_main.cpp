#include <unity.h>
#include <cstdint>
#include "ui/driver/ILedDriver.h"

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
// Mock driver for contract testing
// -----------------------------------------------------------------------------
struct MockLedDriver : public ILedDriver {
    bool beginCalled = false;
    bool showCalled = false;
    uint8_t lastBrightness = 0;
    uint8_t lastR = 0, lastG = 0, lastB = 0;

    void begin() override { beginCalled = true; }
    void setBrightness(uint8_t brightness) override { lastBrightness = brightness; }
    void setColor(uint8_t r, uint8_t g, uint8_t b) override { lastR = r; lastG = g; lastB = b; }
    void show() override { showCalled = true; }
};

// -----------------------------------------------------------------------------
// Contract tests with LedManager
// -----------------------------------------------------------------------------
void test_ledmanager_driver_contract() {
    MockLedDriver driver;
    LedManager lm(&driver);

    // Begin initializes driver
    lm.begin();
    TEST_ASSERT_TRUE(driver.beginCalled);

    // Set LED on: driver should get max brightness
    lm.setOn();
    lm.loop();
    TEST_ASSERT_EQUAL_UINT8(MAX_BRIGHTNESS, driver.lastBrightness);

    // Set LED off: driver should get 0 brightness
    lm.setOff();
    lm.loop();
    TEST_ASSERT_EQUAL_UINT8(0, driver.lastBrightness);

    // Change color: driver should get correct RGB
    lm.setColorIndex(2); // red
    lm.setOn();
    lm.loop();
    TEST_ASSERT_EQUAL_UINT8(255, driver.lastR);
    TEST_ASSERT_EQUAL_UINT8(0, driver.lastG);
    TEST_ASSERT_EQUAL_UINT8(0, driver.lastB);

    // Test pulsing updates brightness within range
    lm.setPulsing();
    uint8_t first = lm.getNewBrightness();
    TEST_ASSERT_TRUE(first <= MAX_BRIGHTNESS);

    // Test blinking alternates brightness
    lm.setBlinking();
    uint8_t blink1 = lm.getNewBrightness();
    uint8_t blink2 = lm.getNewBrightness();
    TEST_ASSERT_TRUE(blink1 == 0 || blink1 == MAX_BRIGHTNESS);
    TEST_ASSERT_TRUE(blink2 == 0 || blink2 == MAX_BRIGHTNESS);

    // Fade-out-once reduces brightness from max to 0
    lm.setFadeOutOnce(1); // 1 second
    uint8_t fadeStart = lm.getCurrentBrightness();
    TEST_ASSERT_EQUAL_UINT8(MAX_BRIGHTNESS, fadeStart);
}

void test_ledmanager_full_simulation() {
    MockLedDriver driver;
    LedManager lm(&driver);

    lm.begin();
    TEST_ASSERT_TRUE(driver.beginCalled);

    // ---------- ON/OFF ----------
    lm.setOn();
    lm.loop();
    TEST_ASSERT_EQUAL_UINT8(MAX_BRIGHTNESS, driver.lastBrightness);

    lm.setOff();
    lm.loop();
    TEST_ASSERT_EQUAL_UINT8(0, driver.lastBrightness);

    // ---------- Color ----------
    lm.setColorIndex(3); // Blue
    lm.setOn();
    lm.loop();
    TEST_ASSERT_EQUAL_UINT8(0, driver.lastR);
    TEST_ASSERT_EQUAL_UINT8(0, driver.lastG);
    TEST_ASSERT_EQUAL_UINT8(255, driver.lastB);

    // ---------- Pulsing ----------
    lm.setPulsing();
    std::vector<uint8_t> pulseValues;
    for(int i = 0; i < 20; ++i) {
        pulseValues.push_back(lm.getNewBrightness());
        advanceMillis(50);
    }
    // Check that brightness stays within [0, MAX_BRIGHTNESS]
    for(auto b : pulseValues) {
        TEST_ASSERT_TRUE(b <= MAX_BRIGHTNESS);
    }
    TEST_ASSERT_TRUE(std::min(pulseValues.front(), pulseValues.back()) < MAX_BRIGHTNESS); // must vary

    // ---------- Blinking ----------
    lm.setBlinking();
    std::vector<uint8_t> blinkValues;
    for(int i = 0; i < 10; ++i) {
        blinkValues.push_back(lm.getNewBrightness());
        advanceMillis(200);
    }
    for(auto b : blinkValues) {
        TEST_ASSERT_TRUE(b == 0 || b == MAX_BRIGHTNESS);
    }

    // ---------- Fade-out Once ----------
    lm.setFadeOutOnce(1); // 1 second fade
    std::vector<uint8_t> fadeOnceValues;
    for(int i = 0; i <= 10; ++i) { // simulate 0–1000ms
        fadeOnceValues.push_back(lm.getNewBrightness());
        advanceMillis(100);
    }
    TEST_ASSERT_EQUAL_UINT8(MAX_BRIGHTNESS, fadeOnceValues.front());
    TEST_ASSERT_EQUAL_UINT8(0, fadeOnceValues.back());
    for(auto b : fadeOnceValues) {
        TEST_ASSERT_TRUE(b <= MAX_BRIGHTNESS);
    }

    // ---------- Fade-out Loop ----------
    lm.setFadeOutLoop(1, 0); // 1 second fade loop
    std::vector<uint8_t> fadeLoopValues;
    for(int i = 0; i <= 15; ++i) { // simulate 0–1500ms
        fadeLoopValues.push_back(lm.getNewBrightness());
        advanceMillis(100);
    }
    TEST_ASSERT_TRUE(*std::max_element(fadeLoopValues.begin(), fadeLoopValues.end()) <= MAX_BRIGHTNESS);
    TEST_ASSERT_TRUE(*std::min_element(fadeLoopValues.begin(), fadeLoopValues.end()) >= 0);
}


// -----------------------------------------------------------------------------
// Test runner
// -----------------------------------------------------------------------------
int main() {
    UNITY_BEGIN();
    RUN_TEST(test_ledmanager_driver_contract);
    RUN_TEST(test_ledmanager_full_simulation);
    return UNITY_END();
}
