#include <Arduino.h>
#include <unity.h>
#include "storage/StorageManager.h"
#include "storage/StorageManager.cpp"

StorageManager storageManager;

// ---------- Test Setup ----------

void setUp(void) {
    // Called before each test
    storageManager.begin(4096);
    storageManager.factoryReset();
}

void tearDown(void) {
    // Called after each test
}

// ---------- Actual Tests ----------

// Basic write/read roundtrip
void test_eeprom_roundtrip(void) {
    const uint32_t testKey = 0xABCD0001;
    uint8_t testData[16];
    for (int i = 0; i < 16; i++) testData[i] = i + 1;

    bool writeOk = storageManager.writeKeyValue(testKey, testData, sizeof(testData));
    TEST_ASSERT_TRUE_MESSAGE(writeOk, "EEPROM write failed");

    uint8_t readBack[16] = {0};
    bool readOk = storageManager.readValueByKey(testKey, readBack, sizeof(readBack));
    TEST_ASSERT_TRUE_MESSAGE(readOk, "EEPROM read failed");

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(testData, readBack, sizeof(testData), "EEPROM data mismatch");
}


// ---------- Test Runner ----------

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    delay(500);

    UNITY_BEGIN();
    RUN_TEST(test_eeprom_roundtrip);
    UNITY_END();
}

void loop() {
    // nothing
}
