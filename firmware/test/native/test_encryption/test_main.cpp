#include <unity.h>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <string>

// -----------------------------------------------------------------------------
// Include class under test (with private access opened for testing)
// -----------------------------------------------------------------------------
#define private public
#define protected public
#include "crypto/Kdf.h"
#undef private
#undef protected
#include "crypto/Kdf.cpp"

#include "crypto/EncryptionManager.h"
#include "crypto/EncryptionManager.cpp"


// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------
/**
 * @brief Print a byte buffer as hexadecimal to stdout (for debugging).
 *
 * @param label Label prefix to print before the data.
 * @param data  Pointer to the byte array.
 * @param len   Length of the byte array.
 */
static void printHex(const char* label, const uint8_t* data, size_t len) {
    printf("%s", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02X", data[i]);
    }
    printf("\n");
}

/**
 * @brief Fill a buffer with sequential values (1, 2, 3, ...).
 *
 * @param buf   Pointer to the buffer to fill.
 * @param len   Number of bytes to write.
 * @param start Starting byte value (default = 1).
 */
static void fillSequential(uint8_t* buf, size_t len, uint8_t start = 1) {
    for (size_t i = 0; i < len; i++) buf[i] = start + i;
}

/**
 * @brief Helper to perform a full encryption/decryption roundtrip test.
 *
 * Encrypts data using a specified seed slot, decrypts it using the same slot,
 * and asserts that the decrypted output matches the original plaintext.
 *
 * @param seedSlot The seed slot number to initialize the EncryptionManager with.
 * @param dataSize The number of bytes to encrypt/decrypt.
 */
static void run_encryption_roundtrip(uint8_t seedSlot, size_t dataSize) {
    EncryptionManager em;

    std::vector<uint8_t> plaintext(dataSize);
    fillSequential(plaintext.data(), dataSize);

    std::vector<uint8_t> ciphertext(dataSize, 0);
    std::vector<uint8_t> decrypted(dataSize, 0);

    // Encrypt
    em.init(seedSlot);
    bool encOK = em.encrypt(ciphertext.data(), plaintext.data(), dataSize);

    // Decrypt
    em.init(seedSlot);  // Reinitialize same slot to regenerate same key/IV
    bool decOK = em.decrypt(decrypted.data(), ciphertext.data(), dataSize);

    // Debug output
    printf("\n[Slot %d] Roundtrip Test (%zu bytes)\n", seedSlot, dataSize);
    printHex("PLAINTEXT:  ", plaintext.data(), dataSize);
    printHex("CIPHERTEXT: ", ciphertext.data(), dataSize);
    printHex("DECRYPTED:  ", decrypted.data(), dataSize);

    // Assertions
    TEST_ASSERT_TRUE_MESSAGE(encOK, "Encryption failed");
    TEST_ASSERT_TRUE_MESSAGE(decOK, "Decryption failed");
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(plaintext.data(), decrypted.data(), dataSize, "Roundtrip mismatch");
}


// -----------------------------------------------------------------------------
// Individual Tests
// -----------------------------------------------------------------------------

/**
 * @test
 * @brief Verifies basic encryption and decryption functionality using static plaintext.
 *
 * This test ensures that a short, fixed string ("Hello, world!") can be encrypted and
 * decrypted successfully within a single session.
 */
void test_basic_encryptionmanager(void) {
    EncryptionManager em;
    em.init(1);

    const char* plaintext = "Hello, world!";
    const size_t len = strlen(plaintext);

    uint8_t encrypted[64] = {0};
    uint8_t decrypted[64] = {0};

    TEST_ASSERT_TRUE(em.encrypt(encrypted, (const uint8_t*)plaintext, len));
    TEST_ASSERT_TRUE(em.decrypt(decrypted, encrypted, len));
    TEST_ASSERT_EQUAL_UINT8_ARRAY((const uint8_t*)plaintext, decrypted, len);
}

/**
 * @test
 * @brief Tests encryption/decryption roundtrip across multiple seed slots.
 *
 * Runs the roundtrip test for 9 different seed slots (0–8) to confirm that
 * slot-based key generation and isolation behave correctly.
 */
void test_multiple_seed_slots(void) {
    const size_t dataSize = 32;
    for (uint8_t slot = 0; slot < 9; ++slot) {
        run_encryption_roundtrip(slot, dataSize);
    }
}

/**
 * @test
 * @brief Tests encryption/decryption with various data lengths.
 *
 * Validates correctness for a range of message sizes, including non-block-aligned lengths.
 */
void test_different_data_sizes(void) {
    const uint8_t slot = 3;
    const std::vector<size_t> sizes = {1, 8, 16, 31, 32, 64, 128};

    for (size_t size : sizes) {
        run_encryption_roundtrip(slot, size);
    }
}

/**
 * @test
 * @brief Tests encryption/decryption using randomized plaintext data.
 *
 * Confirms that the algorithm works correctly for arbitrary, non-deterministic inputs.
 */
void test_randomized_data(void) {
    EncryptionManager em;
    const uint8_t slot = 5;
    const size_t size = 64;

    std::vector<uint8_t> data(size);
    for (auto& b : data) b = rand() % 256;

    std::vector<uint8_t> ciphertext(size, 0);
    std::vector<uint8_t> decrypted(size, 0);

    em.init(slot);
    TEST_ASSERT_TRUE(em.encrypt(ciphertext.data(), data.data(), size));

    em.init(slot);
    TEST_ASSERT_TRUE(em.decrypt(decrypted.data(), ciphertext.data(), size));

    TEST_ASSERT_EQUAL_UINT8_ARRAY(data.data(), decrypted.data(), size);
}

/**
 * @test
 * @brief Verifies that corrupted ciphertext does not successfully decrypt.
 *
 * Encrypts data, flips one byte of the ciphertext, and then attempts decryption.
 * Decryption should either fail or produce a different output than the original plaintext.
 */
void test_corrupted_ciphertext_should_fail(void) {
    EncryptionManager em;
    const uint8_t slot = 4;
    const size_t size = 32;

    std::vector<uint8_t> plaintext(size);
    fillSequential(plaintext.data(), size);

    std::vector<uint8_t> ciphertext(size, 0);
    std::vector<uint8_t> decrypted(size, 0);

    em.init(slot);
    TEST_ASSERT_TRUE(em.encrypt(ciphertext.data(), plaintext.data(), size));

    // Corrupt one byte in ciphertext
    ciphertext[size / 2] ^= 0xFF;

    em.init(slot);
    bool decOK = em.decrypt(decrypted.data(), ciphertext.data(), size);

    // Either decrypt should fail or produce different output
    if (decOK) {
        TEST_ASSERT_NOT_EQUAL(0, memcmp(plaintext.data(), decrypted.data(), size));
    } else {
        TEST_ASSERT_FALSE_MESSAGE(decOK, "Decryption should fail for corrupted ciphertext");
    }
}

/**
 * @test
 * @brief Validates handling of boundary input sizes.
 *
 * Tests two cases:
 * - **Zero-length input (0 bytes):** EncryptionManager does *not* support empty data;
 *   both `encrypt()` and `decrypt()` are expected to return false.
 * - **Large buffer (1024 bytes):** Ensures correct encryption/decryption for large payloads.
 */
void test_boundary_sizes(void) {
    EncryptionManager em;
    const uint8_t slot = 6;

    // --- Test 0 bytes (expected failure) ---
    {
        em.init(slot);
        uint8_t buf[1] = {0};

        bool encOK = em.encrypt(buf, buf, 0);
        bool decOK = em.decrypt(buf, buf, 0);

        TEST_ASSERT_FALSE_MESSAGE(encOK, "Encryption should return false for zero-length input");
        TEST_ASSERT_FALSE_MESSAGE(decOK, "Decryption should return false for zero-length input");
    }

    // --- Test large buffer (expected success) ---
    {
        const size_t size = 1024;
        std::vector<uint8_t> plaintext(size);
        fillSequential(plaintext.data(), size);

        std::vector<uint8_t> ciphertext(size, 0);
        std::vector<uint8_t> decrypted(size, 0);

        em.init(slot);
        TEST_ASSERT_TRUE_MESSAGE(
            em.encrypt(ciphertext.data(), plaintext.data(), size),
            "Encryption failed for large buffer"
        );

        em.init(slot);
        TEST_ASSERT_TRUE_MESSAGE(
            em.decrypt(decrypted.data(), ciphertext.data(), size),
            "Decryption failed for large buffer"
        );

        TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(
            plaintext.data(), decrypted.data(), size,
            "Large buffer roundtrip mismatch"
        );
    }
}

/**
 * @test
 * @brief Ensures that EncryptionManager cannot be used before initialization.
 *
 * Both `encrypt()` and `decrypt()` must return false when called before `init()`.
 * This prevents usage with uninitialized keys or IVs, ensuring predictable failure
 * instead of undefined behavior.
 */
void test_uninitialized_usage_should_fail(void) {
    EncryptionManager em;  // Not initialized
    const size_t size = 32;

    uint8_t plaintext[size];
    fillSequential(plaintext, size);

    uint8_t ciphertext[size] = {0};
    uint8_t decrypted[size] = {0};

    // Encryption should fail
    bool encOK = em.encrypt(ciphertext, plaintext, size);
    TEST_ASSERT_FALSE_MESSAGE(encOK, "Encryption should fail when called before init()");

    // Decryption should fail
    bool decOK = em.decrypt(decrypted, ciphertext, size);
    TEST_ASSERT_FALSE_MESSAGE(decOK, "Decryption should fail when called before init()");
}

/**
 * @test
 * @brief Ensures that calling init() twice properly reinitializes the EncryptionManager.
 *
 * This test verifies that:
 * - Calling init() multiple times resets the internal state.
 * - Data encrypted under one seed slot cannot be decrypted using another.
 * - Reinitialization with the same slot still allows correct roundtrip.
 */
void test_reinitialization_resets_state(void) {
    EncryptionManager em;
    const size_t size = 32;

    std::vector<uint8_t> plaintext(size);
    fillSequential(plaintext.data(), size);

    std::vector<uint8_t> ciphertext(size, 0);
    std::vector<uint8_t> decrypted(size, 0);

    // --- Initialize with first slot (slot A) ---
    const uint8_t slotA = 1;
    em.init(slotA);
    TEST_ASSERT_TRUE_MESSAGE(
        em.encrypt(ciphertext.data(), plaintext.data(), size),
        "Encryption failed with first init (slot A)"
    );

    // --- Re-initialize with a different slot (slot B) ---
    const uint8_t slotB = 2;
    em.init(slotB);
    bool decOK = em.decrypt(decrypted.data(), ciphertext.data(), size);

    // Decryption with a different slot should fail or produce mismatched data
    if (decOK) {
        TEST_ASSERT_NOT_EQUAL_MESSAGE(
            0, memcmp(plaintext.data(), decrypted.data(), size),
            "Data decrypted correctly with wrong slot — state not properly reset!"
        );
    } else {
        TEST_ASSERT_FALSE_MESSAGE(
            decOK, "Decryption should fail when using a different seed slot"
        );
    }

    // --- Reinitialize with the same slot (slot A) and verify it still works ---
    std::fill(decrypted.begin(), decrypted.end(), 0);
    em.init(slotA);
    TEST_ASSERT_TRUE_MESSAGE(
        em.decrypt(decrypted.data(), ciphertext.data(), size),
        "Decryption failed after reinitializing with same slot (slot A)"
    );
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(
        plaintext.data(), decrypted.data(), size,
        "Roundtrip mismatch after reinitialization with same slot"
    );
}

/**
 * @test
 * @brief Verifies deterministic encryption across different instances using the same slot.
 *
 * This test ensures that:
 * - Two separate EncryptionManager instances initialized with the same seed slot
 *   produce the same ciphertext when encrypting the same plaintext.
 * - Decrypting the ciphertext on either instance recovers the original plaintext.
 */
void test_cross_instance_same_slot_consistency(void) {
    const uint8_t slot = 3;
    const size_t size = 32;

    // Prepare deterministic plaintext
    std::vector<uint8_t> plaintext(size);
    fillSequential(plaintext.data(), size);

    // --- Instance A ---
    EncryptionManager emA;
    emA.init(slot);
    std::vector<uint8_t> ciphertextA(size, 0);
    std::vector<uint8_t> decryptedA(size, 0);
    TEST_ASSERT_TRUE_MESSAGE(
        emA.encrypt(ciphertextA.data(), plaintext.data(), size),
        "Encryption failed on instance A"
    );

    // --- Instance B ---
    EncryptionManager emB;
    emB.init(slot);
    std::vector<uint8_t> ciphertextB(size, 0);
    std::vector<uint8_t> decryptedB(size, 0);
    TEST_ASSERT_TRUE_MESSAGE(
        emB.encrypt(ciphertextB.data(), plaintext.data(), size),
        "Encryption failed on instance B"
    );

    // --- Verify ciphertexts match ---
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(
        ciphertextA.data(), ciphertextB.data(), size,
        "Ciphertext mismatch between two instances using same slot"
    );

    // --- Verify decryption works on both instances ---
    TEST_ASSERT_TRUE_MESSAGE(
        emA.decrypt(decryptedA.data(), ciphertextB.data(), size),
        "Decryption failed on instance A using ciphertext from instance B"
    );
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(
        plaintext.data(), decryptedA.data(), size,
        "Decrypted data mismatch on instance A"
    );

    TEST_ASSERT_TRUE_MESSAGE(
        emB.decrypt(decryptedB.data(), ciphertextA.data(), size),
        "Decryption failed on instance B using ciphertext from instance A"
    );
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(
        plaintext.data(), decryptedB.data(), size,
        "Decrypted data mismatch on instance B"
    );
}

/**
* @test Ensures different seed slots produce independent ciphertexts.
*
* This test verifies that encrypting the same plaintext with two different seed slots
* results in different ciphertexts, confirming that slots are isolated.
*/
void test_slot_isolation(void) {
    const size_t size = 32;
    std::vector<uint8_t> plaintext(size);
    fillSequential(plaintext.data(), size);
    std::vector<uint8_t> ct1(size,0), ct2(size,0);

    EncryptionManager em1, em2;
    em1.init(1);
    em2.init(2);
    TEST_ASSERT_TRUE(em1.encrypt(ct1.data(), plaintext.data(), size));
    TEST_ASSERT_TRUE(em2.encrypt(ct2.data(), plaintext.data(), size));

    TEST_ASSERT_NOT_EQUAL(0, memcmp(ct1.data(), ct2.data(), size));
}

/**
* @test Verifies behavior with invalid seed slot.
*
* Since init() is void, we check that encrypt/decrypt fail when the slot was not properly initialized.
*/
void test_invalid_slot(void) {
    EncryptionManager em;
    const size_t size = 16;
    uint8_t buf[size] = {0};

    // Attempt encryption/decryption without proper initialization
    TEST_ASSERT_FALSE_MESSAGE(em.encrypt(buf, buf, size), "Encrypt should fail for invalid slot (not initialized)");
    TEST_ASSERT_FALSE_MESSAGE(em.decrypt(buf, buf, size), "Decrypt should fail for invalid slot (not initialized)");
}

/**
* @test Repeated encryption with same instance and slot produces consistent ciphertext.
*
* Verifies deterministic behavior: encrypting the same plaintext twice with the same instance
* and seed slot results in identical ciphertext.
*/
void test_repeated_encryption_consistency(void) {
    EncryptionManager em;
    const uint8_t slot=1;
    const size_t size=32;
    std::vector<uint8_t> plaintext(size), ct1(size,0), ct2(size,0);
    fillSequential(plaintext.data(), size);

    em.init(slot);
    TEST_ASSERT_TRUE(em.encrypt(ct1.data(), plaintext.data(), size));
    em.init(slot);
    TEST_ASSERT_TRUE(em.encrypt(ct2.data(), plaintext.data(), size));

    TEST_ASSERT_EQUAL_UINT8_ARRAY(ct1.data(), ct2.data(), size);
}


// -----------------------------------------------------------------------------
// Test Runner
// -----------------------------------------------------------------------------
int main() {
    UNITY_BEGIN();
    RUN_TEST(test_basic_encryptionmanager);
    RUN_TEST(test_multiple_seed_slots);
    RUN_TEST(test_different_data_sizes);
    RUN_TEST(test_randomized_data);
    RUN_TEST(test_corrupted_ciphertext_should_fail);
    RUN_TEST(test_boundary_sizes);
    RUN_TEST(test_uninitialized_usage_should_fail);
    RUN_TEST(test_reinitialization_resets_state);
    RUN_TEST(test_cross_instance_same_slot_consistency);
    RUN_TEST(test_slot_isolation);
    RUN_TEST(test_invalid_slot);
    RUN_TEST(test_repeated_encryption_consistency);
    return UNITY_END();
}
