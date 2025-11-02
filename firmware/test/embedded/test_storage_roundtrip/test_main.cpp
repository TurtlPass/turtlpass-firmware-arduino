#include <Arduino.h>
#include <unity.h>
#include "crypto/Kdf.h"
#include "crypto/Kdf.cpp"
#include "crypto/EncryptionManager.h"
#include "crypto/EncryptionManager.cpp"
#include "storage/StorageManager.h"
#include "storage/StorageManager.cpp"

EncryptionManager encryption;
StorageManager storageManager;

const uint32_t BASE_KEY = 0xABCD0000;
const int NUM_SLOTS = 4;

static void test_encryption_roundtrip_slot(uint8_t slot) {
    uint8_t plaintext[16];
    for (int i = 0; i < 16; i++) plaintext[i] = (uint8_t)(slot * 0x10 + i + 1);

    // Initialize encryption for the slot
    encryption.init(slot);

    uint8_t ciphertext[16];
    TEST_ASSERT_TRUE_MESSAGE(encryption.encrypt(ciphertext, plaintext, sizeof(plaintext)),
                             "Encryption failed");

    // Write to EEPROM
    uint32_t key = BASE_KEY | slot;
    TEST_ASSERT_TRUE_MESSAGE(storageManager.writeKeyValue(key, ciphertext, sizeof(ciphertext)),
                             "EEPROM write failed");

    // Read back
    uint8_t readback[16];
    TEST_ASSERT_TRUE_MESSAGE(storageManager.readValueByKey(key, readback, sizeof(readback)),
                             "EEPROM read failed");

    // Verify ciphertext match
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(ciphertext, readback, sizeof(ciphertext),
                                          "Ciphertext mismatch after EEPROM read");

    // Decrypt and verify
    encryption.init(slot);
    uint8_t decrypted[16];
    TEST_ASSERT_TRUE_MESSAGE(encryption.decrypt(decrypted, readback, sizeof(decrypted)),
                             "Decryption failed");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(plaintext, decrypted, sizeof(decrypted),
                                          "Decrypted data does not match original plaintext");
}

void setUp(void) {
    // Called before each test
    storageManager.begin(1024);
    storageManager.factoryReset();
}

void tearDown(void) {
    // Called after each test
}

void test_encryption_roundtrip_slot0(void) { test_encryption_roundtrip_slot(0); }
void test_encryption_roundtrip_slot1(void) { test_encryption_roundtrip_slot(1); }
void test_encryption_roundtrip_slot2(void) { test_encryption_roundtrip_slot(2); }
void test_encryption_roundtrip_slot3(void) { test_encryption_roundtrip_slot(3); }

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    delay(500);

    UNITY_BEGIN();
    RUN_TEST(test_encryption_roundtrip_slot0);
    RUN_TEST(test_encryption_roundtrip_slot1);
    RUN_TEST(test_encryption_roundtrip_slot2);
    RUN_TEST(test_encryption_roundtrip_slot3);
    UNITY_END();
}

void loop() {}
