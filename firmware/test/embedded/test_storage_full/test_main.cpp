#include <Arduino.h>
#include <unity.h>
#include "storage/StorageManager.h"
#include "storage/StorageManager.cpp"

// Test data buffers
uint8_t testData1[] = {0x01, 0x02, 0x03, 0x04};
uint8_t testData2[] = {0x10, 0x20, 0x30, 0x40, 0x50};
uint8_t readBuffer[16];

StorageManager storageManager;

void setUp(void) {
    storageManager.begin(4096);
    storageManager.factoryReset();
}

///////////////////////////////////////////////////////////////
// Test: Initialization and Factory Reset
///////////////////////////////////////////////////////////////
void test_storage_initialization_and_factory_reset() {
    storageManager.begin(4096); // example EEPROM size
    storageManager.factoryReset();

    // After reset, no keys exist
    TEST_ASSERT_FALSE(storageManager.keyExists(0xAAAA1111));
    TEST_ASSERT_FALSE(storageManager.keyExists(0xBBBB2222));
}

///////////////////////////////////////////////////////////////
// Test: Single Entry Write/Read
///////////////////////////////////////////////////////////////
void test_storage_write_and_read_single_entry() {
    storageManager.factoryReset();

    TEST_ASSERT_TRUE(storageManager.writeKeyValue(0xAAAA1111, testData1, sizeof(testData1)));

    memset(readBuffer, 0, sizeof(readBuffer));
    TEST_ASSERT_TRUE(storageManager.readValueByKey(0xAAAA1111, readBuffer, sizeof(testData1)));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testData1, readBuffer, sizeof(testData1));
}

///////////////////////////////////////////////////////////////
// Test: Multiple Entries Write/Read
///////////////////////////////////////////////////////////////
void test_storage_multiple_entries() {
    storageManager.factoryReset();

    TEST_ASSERT_TRUE(storageManager.writeKeyValue(0xAAAA1111, testData1, sizeof(testData1)));
    TEST_ASSERT_TRUE(storageManager.writeKeyValue(0xBBBB2222, testData2, sizeof(testData2)));

    memset(readBuffer, 0, sizeof(readBuffer));
    TEST_ASSERT_TRUE(storageManager.readValueByKey(0xAAAA1111, readBuffer, sizeof(testData1)));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testData1, readBuffer, sizeof(testData1));

    memset(readBuffer, 0, sizeof(readBuffer));
    TEST_ASSERT_TRUE(storageManager.readValueByKey(0xBBBB2222, readBuffer, sizeof(testData2)));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testData2, readBuffer, sizeof(testData2));
}

///////////////////////////////////////////////////////////////
// Test: No-overwrite Behavior
///////////////////////////////////////////////////////////////
void test_storage_no_overwrite_behavior() {
    storageManager.factoryReset();

    TEST_ASSERT_TRUE(storageManager.writeKeyValue(0xAAAA1111, testData1, sizeof(testData1)));
    // Attempting to write same key again should fail
    TEST_ASSERT_FALSE(storageManager.writeKeyValue(0xAAAA1111, testData2, sizeof(testData2)));
}

///////////////////////////////////////////////////////////////
// Test: Invalid Input Protection
///////////////////////////////////////////////////////////////
void test_storage_invalid_input_protection() {
    storageManager.factoryReset();

    TEST_ASSERT_FALSE(storageManager.writeKeyValue(0xAAAA1111, nullptr, sizeof(testData1)));
    TEST_ASSERT_FALSE(storageManager.writeKeyValue(0xAAAA1111, testData1, 0));
    TEST_ASSERT_FALSE(storageManager.readValueByKey(0xAAAA1111, nullptr, sizeof(testData1)));
    TEST_ASSERT_FALSE(storageManager.readValueByKey(0xAAAA1111, readBuffer, 0));
}

///////////////////////////////////////////////////////////////
// Test: Overflow Protection
///////////////////////////////////////////////////////////////
void test_storage_overflow_protection() {
    storageManager.factoryReset();

    uint16_t oversized = storageManager.capacity() - HEADER_SIZE + 10;
    uint8_t* largeBuffer = new uint8_t[oversized];
    memset(largeBuffer, 0xAA, oversized);

    TEST_ASSERT_FALSE(storageManager.writeKeyValue(0xDEADBEAF, largeBuffer, oversized));
    delete[] largeBuffer;
}

///////////////////////////////////////////////////////////////
// Test: Magic Header Recovery (implicit)
///////////////////////////////////////////////////////////////
void test_storage_magic_header_recovery() {
    storageManager.factoryReset();

    // Call begin() again simulates reinitialization
    storageManager.begin(storageManager.capacity());

    TEST_ASSERT_TRUE(storageManager.writeKeyValue(0xAAAA1111, testData1, sizeof(testData1)));
    memset(readBuffer, 0, sizeof(readBuffer));
    TEST_ASSERT_TRUE(storageManager.readValueByKey(0xAAAA1111, readBuffer, sizeof(testData1)));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testData1, readBuffer, sizeof(testData1));
}


//////////////////////////////////////////////////////////
// Test Runner
//////////////////////////////////////////////////////////

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    delay(500);

    UNITY_BEGIN();
    RUN_TEST(test_storage_initialization_and_factory_reset);
    RUN_TEST(test_storage_write_and_read_single_entry);
    RUN_TEST(test_storage_multiple_entries);
    RUN_TEST(test_storage_no_overwrite_behavior);
    RUN_TEST(test_storage_invalid_input_protection);
    RUN_TEST(test_storage_overflow_protection);
    RUN_TEST(test_storage_magic_header_recovery);
    UNITY_END();
}

void loop() {
    // Nothing
}
