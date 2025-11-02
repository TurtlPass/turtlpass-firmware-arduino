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

static void fillTestSeed(uint8_t* buf, size_t len, uint8_t base = 0x10) {
    for (size_t i = 0; i < len; i++) buf[i] = static_cast<uint8_t>(base + i);
}

static bool buffersEqual(const uint8_t* a, const uint8_t* b, size_t len) {
    return memcmp(a, b, len) == 0;
}

// ---------- Setup / Teardown ----------

void setUp(void) {
    seedManager.begin();
    seedManager.factoryReset();
}

void tearDown(void) {
    // nothing
}

// ---------- Crypto Tests ----------

// Deterministic encryption: same slot, same seed → same ciphertext
void test_seedmanager_deterministic_encryption(void) {
    uint8_t seed[SeedManager::SEED_SIZE];
    fillTestSeed(seed, SeedManager::SEED_SIZE, 0x20);

    TEST_ASSERT_EQUAL_UINT8((uint8_t)SeedManager::SeedInitResult::OK,
                            (uint8_t)seedManager.initializeSeed(1, seed, SeedManager::SEED_SIZE));

    uint8_t firstRead[SeedManager::SEED_SIZE];
    TEST_ASSERT_TRUE(seedManager.getSeed(1, firstRead, SeedManager::SEED_SIZE));

    // Reset SeedManager and read again
    seedManager.factoryReset();
    seedManager.begin();

    TEST_ASSERT_EQUAL_UINT8((uint8_t)SeedManager::SeedInitResult::OK,
                            (uint8_t)seedManager.initializeSeed(1, seed, SeedManager::SEED_SIZE));

    uint8_t secondRead[SeedManager::SEED_SIZE];
    TEST_ASSERT_TRUE(seedManager.getSeed(1, secondRead, SeedManager::SEED_SIZE));

    TEST_ASSERT_TRUE_MESSAGE(buffersEqual(firstRead, secondRead, SeedManager::SEED_SIZE),
                             "Deterministic encryption failed: ciphertext mismatch");
}

// Cross-slot isolation: different slots produce different ciphertexts
void test_seedmanager_slot_isolation(void) {
    const int numSlots = 2;
    uint8_t seeds[numSlots][SeedManager::SEED_SIZE];

    for (int i = 0; i < numSlots; i++)
        fillTestSeed(seeds[i], SeedManager::SEED_SIZE, 0x30 + i * 0x10);

    // Initialize slots starting from 1
    for (int i = 0; i < numSlots; i++) {
        TEST_ASSERT_EQUAL_UINT8((uint8_t)SeedManager::SeedInitResult::OK,
                                (uint8_t)seedManager.initializeSeed(i + 1, seeds[i], SeedManager::SEED_SIZE));
    }

    // Read back ciphertexts
    uint8_t read1[SeedManager::SEED_SIZE], read2[SeedManager::SEED_SIZE];
    TEST_ASSERT_TRUE(seedManager.getSeed(1, read1, SeedManager::SEED_SIZE));
    TEST_ASSERT_TRUE(seedManager.getSeed(2, read2, SeedManager::SEED_SIZE));

    TEST_ASSERT_FALSE_MESSAGE(buffersEqual(read1, read2, SeedManager::SEED_SIZE),
                              "Cross-slot isolation failed: ciphertexts should differ");
}

// Decryption correctness: decrypt yields stored hash (SHA-512 of seed)
void test_seedmanager_decryption_correctness(void) {
    uint8_t seed[SeedManager::SEED_SIZE];
    fillTestSeed(seed, SeedManager::SEED_SIZE, 0x40);

    // Initialize the SeedManager with the given seed
    TEST_ASSERT_EQUAL_UINT8(
        (uint8_t)SeedManager::SeedInitResult::OK,
        (uint8_t)seedManager.initializeSeed(1, seed, SeedManager::SEED_SIZE)
    );

    // Retrieve the decrypted seed data (which should be the stored hash)
    uint8_t decrypted[SHA512::HASH_SIZE];
    TEST_ASSERT_TRUE(seedManager.getSeed(1, decrypted, sizeof(decrypted)));

    // Compute expected SHA-512 hash of the original seed
    uint8_t expectedHash[SHA512::HASH_SIZE];
    SHA512 sha;
    sha.reset();
    sha.update(seed, SeedManager::SEED_SIZE);
    sha.finalize(expectedHash, sizeof(expectedHash));

    // Verify decrypted data matches expected hash
    TEST_ASSERT_TRUE_MESSAGE(
        buffersEqual(expectedHash, decrypted, SHA512::HASH_SIZE),
        "Decryption failed: output does not match expected SHA-512(seed)"
    );
}

// Corruption detection: modifying ciphertext yields decryption failure
void test_seedmanager_corrupted_ciphertext(void) {
    uint8_t seed[SeedManager::SEED_SIZE];
    fillTestSeed(seed, SeedManager::SEED_SIZE, 0x50);

    TEST_ASSERT_EQUAL_UINT8((uint8_t)SeedManager::SeedInitResult::OK,
                            (uint8_t)seedManager.initializeSeed(1, seed, SeedManager::SEED_SIZE));

    // Simulate corruption
    uint8_t corrupted[SeedManager::SEED_SIZE];
    TEST_ASSERT_TRUE(seedManager.getSeed(1, corrupted, SeedManager::SEED_SIZE));
    corrupted[0] ^= 0xFF; // flip first byte

    EncryptionManager enc;
    enc.init(1);
    uint8_t decrypted[SeedManager::SEED_SIZE];
    enc.decrypt(decrypted, corrupted, SeedManager::SEED_SIZE); // may still return true

    // Check that decrypted != original
    TEST_ASSERT_FALSE_MESSAGE(buffersEqual(seed, decrypted, SeedManager::SEED_SIZE),
                              "Decryption of corrupted ciphertext should NOT match original seed");
}


// ---------- Test runner ----------

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    delay(500);

    UNITY_BEGIN();
    RUN_TEST(test_seedmanager_deterministic_encryption);
    RUN_TEST(test_seedmanager_slot_isolation);
    RUN_TEST(test_seedmanager_decryption_correctness);
    RUN_TEST(test_seedmanager_corrupted_ciphertext);
    UNITY_END();
}

void loop() {}
