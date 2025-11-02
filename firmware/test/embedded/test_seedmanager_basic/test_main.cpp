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

// Global instance under test
SeedManager seedManager;

// --- Helper utilities ---

static void fillTestSeed(uint8_t* buf, size_t len, uint8_t base = 0x11) {
    for (size_t i = 0; i < len; i++) buf[i] = (uint8_t)(base + i);
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

// --- Unity setUp / tearDown ---

void setUp(void) {
    seedManager.begin();
    seedManager.factoryReset();
}

void tearDown(void) {
    // nothing for now
}

// --- Tests ---

void test_seedmanager_initialize_and_get(void) {
    uint8_t seed[SeedManager::SEED_SIZE];
    fillTestSeed(seed, sizeof(seed));

    auto res = seedManager.initializeSeed(1, seed, sizeof(seed));
    TEST_ASSERT_EQUAL_UINT8_MESSAGE((uint8_t)SeedManager::SeedInitResult::OK, (uint8_t)res, "Seed initialization failed");

    uint8_t readBack[SHA512::HASH_SIZE] = {0};
    bool ok = seedManager.getSeed(1, readBack, sizeof(readBack));
    TEST_ASSERT_TRUE_MESSAGE(ok, "getSeed() failed");

    uint8_t expectedHash[SHA512::HASH_SIZE];
    computeSeedHash(seed, sizeof(seed), expectedHash);
    TEST_ASSERT_TRUE_MESSAGE(buffersEqual(expectedHash, readBack, SHA512::HASH_SIZE),
                             "Roundtrip seed mismatch");
}

void test_seedmanager_multiple_slots(void) {
    const int numSlots = 9;
    uint8_t seeds[numSlots][SeedManager::SEED_SIZE];

    // Initialize seeds (slots 1..9)
    for (int i = 0; i < numSlots; i++) {
        fillTestSeed(seeds[i], SeedManager::SEED_SIZE, 0x20 + i * 0x10);
        auto res = seedManager.initializeSeed(i + 1, seeds[i], SeedManager::SEED_SIZE);
        TEST_ASSERT_EQUAL_UINT8((uint8_t)SeedManager::SeedInitResult::OK, (uint8_t)res);
    }

    // Verify each one retrieves correctly
    for (int i = 0; i < numSlots; i++) {
        uint8_t out[SHA512::HASH_SIZE];
        bool ok = seedManager.getSeed(i + 1, out, sizeof(out));
        TEST_ASSERT_TRUE(ok);

        uint8_t expectedHash[SHA512::HASH_SIZE];
        computeSeedHash(seeds[i], SeedManager::SEED_SIZE, expectedHash);
        TEST_ASSERT_TRUE(buffersEqual(expectedHash, out, SHA512::HASH_SIZE));
    }
}

void test_seedmanager_invalid_slot(void) {
    uint8_t out[SHA512::HASH_SIZE];
    bool ok = seedManager.getSeed(33, out, sizeof(out)); // assume out-of-range
    TEST_ASSERT_FALSE_MESSAGE(ok, "Expected getSeed() to fail on invalid slot");
}

void test_seedmanager_wrong_size(void) {
    uint8_t seed[SeedManager::SEED_SIZE];
    fillTestSeed(seed, sizeof(seed));

    auto res = seedManager.initializeSeed(1, seed, SeedManager::SEED_SIZE);
    TEST_ASSERT_EQUAL_UINT8((uint8_t)SeedManager::SeedInitResult::OK, (uint8_t)res);

    uint8_t out[SHA512::HASH_SIZE / 2]; // intentionally too small
    bool ok = seedManager.getSeed(1, out, sizeof(out)); // smaller buffer
    TEST_ASSERT_FALSE_MESSAGE(ok, "Expected getSeed() to fail with small buffer");
}

// --- Test runner ---

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    delay(500);

    UNITY_BEGIN();
    RUN_TEST(test_seedmanager_initialize_and_get);
    RUN_TEST(test_seedmanager_multiple_slots);
    RUN_TEST(test_seedmanager_invalid_slot);
    RUN_TEST(test_seedmanager_wrong_size);
    UNITY_END();
}

void loop() {}
