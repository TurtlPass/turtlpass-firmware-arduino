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

// ---------- Setup / Teardown ----------
void setUp(void) {
    seedManager.begin();
    seedManager.factoryReset();
}

void tearDown(void) {}

// ---------- Tests ----------

void test_seedmanager_invalid_input(void) {
    // nullptr seed pointer
    auto res1 = seedManager.initializeSeed(0, nullptr, SeedManager::SEED_SIZE);
    TEST_ASSERT_EQUAL_UINT8((uint8_t)SeedManager::SeedInitResult::INVALID_INPUT, (uint8_t)res1);

    // zero-length seed
    uint8_t seed[SeedManager::SEED_SIZE];
    fillTestSeed(seed, SeedManager::SEED_SIZE, 0x10);
    auto res2 = seedManager.initializeSeed(0, seed, 0);
    TEST_ASSERT_EQUAL_UINT8((uint8_t)SeedManager::SeedInitResult::INVALID_INPUT, (uint8_t)res2);
}

void test_seedmanager_max_slot(void) {
    const uint8_t maxSlot = 9;
    uint8_t seed[SeedManager::SEED_SIZE];
    fillTestSeed(seed, SeedManager::SEED_SIZE, 0x30);

    auto res = seedManager.initializeSeed(maxSlot, seed, SeedManager::SEED_SIZE);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE((uint8_t)SeedManager::SeedInitResult::OK, (uint8_t)res, "Max slot init failed");

    uint8_t out[SHA512::HASH_SIZE];
    bool ok = seedManager.getSeed(maxSlot, out, sizeof(out));
    TEST_ASSERT_TRUE_MESSAGE(ok, "Max slot getSeed failed");

    uint8_t expectedHash[SHA512::HASH_SIZE];
    computeSeedHash(seed, SeedManager::SEED_SIZE, expectedHash);
    TEST_ASSERT_TRUE_MESSAGE(buffersEqual(expectedHash, out, SHA512::HASH_SIZE), "Max slot roundtrip mismatch");
}

void test_seedmanager_invalid_slots(void) {
    uint8_t seed[SeedManager::SEED_SIZE];
    fillTestSeed(seed, SeedManager::SEED_SIZE, 0x40);

    // Out-of-range slot
    auto resNeg = seedManager.initializeSeed(255, seed, SeedManager::SEED_SIZE);
    TEST_ASSERT_EQUAL_UINT8((uint8_t)SeedManager::SeedInitResult::INVALID_SLOT, (uint8_t)resNeg);

    uint8_t out[SHA512::HASH_SIZE];
    bool ok = seedManager.getSeed(255, out, sizeof(out));
    TEST_ASSERT_FALSE_MESSAGE(ok, "getSeed should fail for invalid slot");
}

void test_seedmanager_cross_slot_isolation(void) {
    const int numSlots = 9;
    uint8_t seeds[numSlots][SeedManager::SEED_SIZE];

    // Initialize each slot
    for (int i = 0; i < numSlots; i++) {
        fillTestSeed(seeds[i], SeedManager::SEED_SIZE, 0x50 + i * 0x10);
        auto res = seedManager.initializeSeed(i + 1, seeds[i], SeedManager::SEED_SIZE);
        TEST_ASSERT_EQUAL_UINT8_MESSAGE((uint8_t)SeedManager::SeedInitResult::OK, (uint8_t)res, "Slot init failed");
    }

    // Verify each slot independently
    for (int i = 0; i < numSlots; i++) {
        uint8_t out[SHA512::HASH_SIZE];
        bool ok = seedManager.getSeed(i + 1, out, sizeof(out));
        TEST_ASSERT_TRUE(ok);

        uint8_t expectedHash[SHA512::HASH_SIZE];
        computeSeedHash(seeds[i], SeedManager::SEED_SIZE, expectedHash);
        TEST_ASSERT_TRUE(buffersEqual(expectedHash, out, SHA512::HASH_SIZE));
    }
}

void test_seedmanager_overwrite_seed(void) {
    uint8_t seed1[SeedManager::SEED_SIZE], seed2[SeedManager::SEED_SIZE];
    fillTestSeed(seed1, SeedManager::SEED_SIZE, 0x60);
    fillTestSeed(seed2, SeedManager::SEED_SIZE, 0x70);

    auto res1 = seedManager.initializeSeed(1, seed1, SeedManager::SEED_SIZE);
    TEST_ASSERT_EQUAL_UINT8((uint8_t)SeedManager::SeedInitResult::OK, (uint8_t)res1);

    auto res2 = seedManager.initializeSeed(1, seed2, SeedManager::SEED_SIZE);
    TEST_ASSERT_EQUAL_UINT8((uint8_t)SeedManager::SeedInitResult::ALREADY_POPULATED, (uint8_t)res2);

    // Verify original seed is intact
    uint8_t out[SHA512::HASH_SIZE];
    bool ok = seedManager.getSeed(1, out, sizeof(out));
    TEST_ASSERT_TRUE(ok);

    uint8_t expectedHash[SHA512::HASH_SIZE];
    computeSeedHash(seed1, SeedManager::SEED_SIZE, expectedHash);
    TEST_ASSERT_TRUE(buffersEqual(expectedHash, out, SHA512::HASH_SIZE));
}

void test_seedmanager_factory_reset(void) {
    uint8_t seed[SeedManager::SEED_SIZE];
    fillTestSeed(seed, SeedManager::SEED_SIZE);

    seedManager.initializeSeed(1, seed, SeedManager::SEED_SIZE);

    seedManager.factoryReset();

    uint8_t out[SHA512::HASH_SIZE];
    bool ok = seedManager.getSeed(1, out, sizeof(out));
    TEST_ASSERT_FALSE_MESSAGE(ok, "Seed should not exist after factoryReset");
}

// ---------- Test runner ----------
void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    delay(500);

    UNITY_BEGIN();
    RUN_TEST(test_seedmanager_invalid_input);
    RUN_TEST(test_seedmanager_max_slot);
    RUN_TEST(test_seedmanager_invalid_slots);
    RUN_TEST(test_seedmanager_cross_slot_isolation);
    RUN_TEST(test_seedmanager_overwrite_seed);
    RUN_TEST(test_seedmanager_factory_reset);
    UNITY_END();
}

void loop() {}
