#include <Arduino.h>
#include <unity.h>

#include "crypto/Kdf.h"
#include "crypto/Kdf.cpp"
#include "crypto/EncryptionManager.h"
#include "crypto/EncryptionManager.cpp"
#include "storage/StorageManager.h"
#include "storage/StorageManager.cpp"
#include "storage/SeedManager.h"
#include "storage/SeedManager.cpp"
#include "proto/turtlpass.pb.h"

SeedManager seedManager;

// ---------- Utilities ----------
static void fillTestSeed(uint8_t* buf, size_t len, uint8_t base = 0x10) {
    for (size_t i = 0; i < len; i++) buf[i] = static_cast<uint8_t>(base + i);
}

static bool buffersEqual(const uint8_t* a, const uint8_t* b, size_t len) {
    return memcmp(a, b, len) == 0;
}

// Compute SHA-512(seed) helper
static void computeSeedHash(const uint8_t* seed, size_t len, uint8_t* outHash) {
    SHA512 sha;
    sha.reset();
    sha.update(seed, len);
    sha.finalize(outHash, SHA512::HASH_SIZE);
}

// ---------- Setup ----------
void setUp() {
    seedManager.begin();
    seedManager.factoryReset();
}

void tearDown() {}

// ---------- EEPROM Tests ----------

// Simple write & readback
void test_seedmanager_eeprom_roundtrip(void) {
    uint8_t seed[SeedManager::SEED_SIZE];
    fillTestSeed(seed, SeedManager::SEED_SIZE, 0x20);

    TEST_ASSERT_EQUAL_UINT8(
        (uint8_t)SeedManager::SeedInitResult::OK,
        (uint8_t)seedManager.initializeSeed(1, seed, SeedManager::SEED_SIZE)
    );

    uint8_t readback[SHA512::HASH_SIZE];
    TEST_ASSERT_TRUE(seedManager.getSeed(1, readback, sizeof(readback)));

    uint8_t expectedHash[SHA512::HASH_SIZE];
    computeSeedHash(seed, SeedManager::SEED_SIZE, expectedHash);

    TEST_ASSERT_TRUE_MESSAGE(
        buffersEqual(expectedHash, readback, SHA512::HASH_SIZE),
        "EEPROM roundtrip failed: decrypted data != SHA-512(seed)"
    );
}

// Multiple slot writes
void test_seedmanager_eeprom_multi_slot(void) {
    const int numSlots = 9;

    for (int i = 0; i < numSlots; i++) {
        uint8_t seed[SeedManager::SEED_SIZE];
        fillTestSeed(seed, SeedManager::SEED_SIZE, 0x30 + i * 0x10);

        TEST_ASSERT_EQUAL_UINT8(
            (uint8_t)SeedManager::SeedInitResult::OK,
            (uint8_t)seedManager.initializeSeed(i + 1, seed, SeedManager::SEED_SIZE)
        );

        uint8_t readback[SHA512::HASH_SIZE];
        TEST_ASSERT_TRUE(seedManager.getSeed(i + 1, readback, sizeof(readback)));

        uint8_t expectedHash[SHA512::HASH_SIZE];
        computeSeedHash(seed, SeedManager::SEED_SIZE, expectedHash);

        TEST_ASSERT_TRUE_MESSAGE(
            buffersEqual(expectedHash, readback, SHA512::HASH_SIZE),
            "Multi-slot test failed: decrypted data != SHA-512(seed)"
        );
    }
}

// Factory reset clears EEPROM
void test_seedmanager_eeprom_factory_reset(void) {
    uint8_t seed[SeedManager::SEED_SIZE];
    fillTestSeed(seed, SeedManager::SEED_SIZE, 0x40);

    TEST_ASSERT_EQUAL_UINT8(
        (uint8_t)SeedManager::SeedInitResult::OK,
        (uint8_t)seedManager.initializeSeed(1, seed, SeedManager::SEED_SIZE)
    );

    seedManager.factoryReset();

    uint8_t buf[SHA512::HASH_SIZE];
    TEST_ASSERT_FALSE(seedManager.getSeed(1, buf, sizeof(buf)));
}

// Prevent overwrite of existing slot
void test_seedmanager_eeprom_no_overwrite(void) {
    uint8_t seed[SeedManager::SEED_SIZE];
    fillTestSeed(seed, SeedManager::SEED_SIZE, 0x50);

    TEST_ASSERT_EQUAL_UINT8(
        (uint8_t)SeedManager::SeedInitResult::OK,
        (uint8_t)seedManager.initializeSeed(1, seed, SeedManager::SEED_SIZE)
    );

    uint8_t newSeed[SeedManager::SEED_SIZE];
    fillTestSeed(newSeed, SeedManager::SEED_SIZE, 0x60);

    TEST_ASSERT_EQUAL_UINT8(
        (uint8_t)SeedManager::SeedInitResult::ALREADY_POPULATED,
        (uint8_t)seedManager.initializeSeed(1, newSeed, SeedManager::SEED_SIZE)
    );
}

// Small buffer fails
void test_seedmanager_eeprom_small_buffer(void) {
    uint8_t seed[SeedManager::SEED_SIZE];
    fillTestSeed(seed, SeedManager::SEED_SIZE, 0x70);

    TEST_ASSERT_EQUAL_UINT8(
        (uint8_t)SeedManager::SeedInitResult::OK,
        (uint8_t)seedManager.initializeSeed(1, seed, SeedManager::SEED_SIZE)
    );

    uint8_t buf[SHA512::HASH_SIZE / 2]; // intentionally too small
    TEST_ASSERT_FALSE(seedManager.getSeed(1, buf, sizeof(buf)));
}

// ---------- Test runner ----------
void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    delay(500);

    UNITY_BEGIN();
    RUN_TEST(test_seedmanager_eeprom_roundtrip);
    RUN_TEST(test_seedmanager_eeprom_multi_slot);
    RUN_TEST(test_seedmanager_eeprom_factory_reset);
    RUN_TEST(test_seedmanager_eeprom_no_overwrite);
    RUN_TEST(test_seedmanager_eeprom_small_buffer);
    UNITY_END();
}

void loop() {}
