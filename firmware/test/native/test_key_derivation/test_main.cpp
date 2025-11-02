#include <unity.h>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <map>

// -----------------------------------------------------------------------------
// Include class under test (with private access opened for testing)
// -----------------------------------------------------------------------------
#define private public
#define protected public
#include "crypto/Kdf.h"
#undef private
#undef protected

#include "crypto/Kdf.cpp" // explicit include

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------
static void print_hex(const char* label, const uint8_t* data, size_t len) {
    printf("%s [%zu bytes]: ", label, len);
    for (size_t i = 0; i < len; i++) {
        printf("0x%02X", data[i]);
        if (i + 1 < len) printf(", ");
    }
    printf("\n");
}


// -----------------------------------------------------------------------------
// Tests
// -----------------------------------------------------------------------------

//////////
// HKDF //
//////////

void test_hkdf_various_lengths(void) {
    Kdf kdf;
    uint8_t src[]  = {0x01, 0x02, 0x03, 0x04, 0x05};
    uint8_t salt[] = {0x0A, 0x0B, 0x0C, 0x0D};

    size_t lengths[] = {8, 16, 24, 32};
    for (size_t len : lengths) {
        std::vector<uint8_t> dst(len, 0);

        // Call hkdf (returns void)
        kdf.hkdf(dst.data(), len, src, sizeof(src), salt, sizeof(salt));

        // Optionally, check deterministic property or print
        printf("HKDF output (%zu bytes): ", len);
        for (size_t i = 0; i < len; i++) printf("%02X ", dst[i]);
        printf("\n");

        // Simple non-zero check
        bool non_zero = false;
        for (auto b : dst) if (b != 0) { non_zero = true; break; }
        TEST_ASSERT_TRUE(non_zero);
    }
}

void test_hkdf_deterministic(void) {
    Kdf kdf;
    uint8_t src[]  = {0x01, 0x02, 0x03};
    uint8_t salt[] = {0x0A, 0x0B};
    uint8_t dst1[16] = {0};
    uint8_t dst2[16] = {0};

    kdf.hkdf(dst1, sizeof(dst1), src, sizeof(src), salt, sizeof(salt));
    kdf.hkdf(dst2, sizeof(dst2), src, sizeof(src), salt, sizeof(salt));

    // Outputs must be identical
    TEST_ASSERT_EQUAL_UINT8_ARRAY(dst1, dst2, sizeof(dst1));
}

/////////////////
// derivateKey //
/////////////////

void test_derivateKey_null_args() { 
    Kdf kdf;
    uint8_t dst[16];
    TEST_ASSERT_FALSE(kdf.derivateKey(nullptr, 10, (char*)"input", "seed"));
    TEST_ASSERT_FALSE(kdf.derivateKey(dst, 10, nullptr, "seed"));
    TEST_ASSERT_FALSE(kdf.derivateKey(dst, 10, (char*)"input", nullptr));
}

void test_derivateKey_various_lengths() {
    Kdf kdf;
    char input[] = "test";
    const char* seed = "1f517340d371cbf900369c48085c3a253821f77ce0adc494c2d8937068f91d7e";

    // Expected outputs precomputed for deterministic testing
    std::map<size_t, std::vector<uint8_t>> expected_outputs = {
        {8,  {0xCF, 0xD7, 0x00, 0x0A, 0xFD, 0x84, 0xAF, 0xEA}},
        {16, {0xCF, 0xD7, 0x00, 0x0A, 0xFD, 0x84, 0xAF, 0xEA, 0x7E, 0x5E, 0x39, 0x1E, 0x50, 0x6E, 0xA2, 0xAD}},
        {24, {0xCF, 0xD7, 0x00, 0x0A, 0xFD, 0x84, 0xAF, 0xEA, 0x7E, 0x5E, 0x39, 0x1E, 0x50, 0x6E, 0xA2, 0xAD, 0x76, 0x13, 0x65, 0xC3, 0x73, 0x20, 0x2A, 0x2A}},
        {32, {0xCF, 0xD7, 0x00, 0x0A, 0xFD, 0x84, 0xAF, 0xEA, 0x7E, 0x5E, 0x39, 0x1E, 0x50, 0x6E, 0xA2, 0xAD, 0x76, 0x13, 0x65, 0xC3, 0x73, 0x20, 0x2A, 0x2A, 0xB0, 0xD9, 0x43, 0xF2, 0x23, 0xEC, 0x7F, 0xEC}}
    };

    for (auto &[len, expected] : expected_outputs) {
        std::vector<uint8_t> dst(len);
        bool result = kdf.derivateKey(dst.data(), dst.size(), input, seed);

        //print_hex("Derived key", dst.data(), dst.size());

        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expected.data(), dst.data(), len);
    }
}

void test_derivateKey_all_output_lengths(void) {
    Kdf kdf;
    char input[] = "test";
    const char* seed = "1f517340d371cbf900369c48085c3a253821f77ce0adc494c2d8937068f91d7e";

    for (size_t len = 1; len <= 128; ++len) {
        std::vector<uint8_t> dst(len, 0); // no extra byte needed for binary data

        bool result = kdf.derivateKey(dst.data(), len, input, seed);

        //printf("[%3zu bytes]: ", len);

        TEST_ASSERT_TRUE_MESSAGE(result, "derivateKey() returned false");

        // Check that output buffer is non-zero
        bool non_zero = false;
        for (auto b : dst) {
            if (b != 0) { non_zero = true; break; }
        }
        TEST_ASSERT_TRUE_MESSAGE(non_zero, "Derived key is all zeros");
    }
}

//////////////////
// derivatePass //
//////////////////

void test_derivatePass_null_args() {
    Kdf kdf;
    uint8_t dst[50];
    TEST_ASSERT_FALSE(kdf.derivatePass(nullptr, 10, (char*)"input", "seed"));
    TEST_ASSERT_FALSE(kdf.derivatePass(dst, 10, nullptr, "seed"));
    TEST_ASSERT_FALSE(kdf.derivatePass(dst, 10, (char*)"input", nullptr));
}

void test_derivatePass_various_lengths() {
    Kdf kdf;
    char input[] = "test";
    const char* seed = "1f517340d371cbf900369c48085c3a253821f77ce0adc494c2d8937068f91d7e";

    std::map<size_t, std::string> expected_passes = {
        {16, "RHTHfWQuAp5jtbJ6"},
        {32, "epIR6t0t70pcQCYX6aqh9CIg7N6xQnmO"},
        {48, "1gWxB8EG20o9CwHtIzelCqkS9CY3mrpWY8gtJVzUvky6whJf"}
    };

    for (auto &[len, expected] : expected_passes) {
        std::vector<uint8_t> dst(len + 1, 0);
        bool result = kdf.derivatePass(dst.data(), len, input, seed);

        //printf("[%zu bytes]: %s\n", len, dst.data());

        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQUAL_STRING(expected.c_str(), reinterpret_cast<char*>(dst.data()));
    }
}

void test_derivatePass_all_output_lengths(void) {
    Kdf kdf;
    char input[] = "test";
    const char* seed = "1f517340d371cbf900369c48085c3a253821f77ce0adc494c2d8937068f91d7e";

    for (size_t len = 1; len <= 128; ++len) {
        std::vector<uint8_t> dst(len + 1, 0); // +1 for null terminator

        bool result = kdf.derivatePass(dst.data(), len, input, seed);

        //printf("[%3zu chars]: %s\n", len, dst.data());

        TEST_ASSERT_TRUE_MESSAGE(result, "derivatePass() returned false");

        size_t derived_len = std::strlen(reinterpret_cast<char*>(dst.data()));
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(len, derived_len, "Derived password length mismatch");
    }
}

/////////////////////////////
// derivatePassWithSymbols //
/////////////////////////////

void test_derivatePassWithSymbols_null_args() {
     Kdf kdf;
    uint8_t dst[50];
    TEST_ASSERT_FALSE(kdf.derivatePassWithSymbols(nullptr, 10, (char*)"input", "seed"));
    TEST_ASSERT_FALSE(kdf.derivatePassWithSymbols(dst, 10, nullptr, "seed"));
    TEST_ASSERT_FALSE(kdf.derivatePassWithSymbols(dst, 10, (char*)"input", nullptr));
}

void test_derivatePassWithSymbols_various_lengths() {
    Kdf kdf;
    char input[] = "test";
    const char* seed = "1f517340d371cbf900369c48085c3a253821f77ce0adc494c2d8937068f91d7e";

    std::map<size_t, std::string> expected_passes = {
        {16, "m<oA8]c+#+BEZfFD"},
        {32, "u3ItyQ|e5Y?XiH=Ny\"o]D4UEx:4z[nW2"},
        {48, "0?t5.C5.Y`5qwn^jZEwXObbbv\"_+L xD==40/U@/T4s'pe(e"}
    };

    for (auto &[len, expected] : expected_passes) {
        std::vector<uint8_t> dst(len + 1, 0);
        bool result = kdf.derivatePassWithSymbols(dst.data(), len, input, seed);

        //printf("[%zu bytes]: %s\n", len, dst.data());

        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQUAL_STRING(expected.c_str(), reinterpret_cast<char*>(dst.data()));
    }
}

void test_derivatePassWithSymbols_all_output_lengths(void) {
    Kdf kdf;
    char input[] = "test";
    const char* seed = "1f517340d371cbf900369c48085c3a253821f77ce0adc494c2d8937068f91d7e";

    for (size_t len = 1; len <= 128; ++len) {
        std::vector<uint8_t> dst(len + 1, 0); // +1 for null terminator

        bool result = kdf.derivatePassWithSymbols(dst.data(), len, input, seed);

        //printf("[%3zu chars]: %s\n", len, dst.data());

        TEST_ASSERT_TRUE_MESSAGE(result, "derivatePassWithSymbols() returned false");

        size_t derived_len = std::strlen(reinterpret_cast<char*>(dst.data()));
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(len, derived_len, "Derived password length mismatch");
    }
}

/////////////////////////////
// derivatePassLettersOnly //
/////////////////////////////

void test_derivatePassLettersOnly_null_args() {
     Kdf kdf;
    uint8_t dst[50];
    TEST_ASSERT_FALSE(kdf.derivatePassLettersOnly(nullptr, 10, (char*)"input", "seed"));
    TEST_ASSERT_FALSE(kdf.derivatePassLettersOnly(dst, 10, nullptr, "seed"));
    TEST_ASSERT_FALSE(kdf.derivatePassLettersOnly(dst, 10, (char*)"input", nullptr));
}

void test_derivatePassLettersOnly_various_lengths() {
    Kdf kdf;
    char input[] = "test";
    const char* seed = "1f517340d371cbf900369c48085c3a253821f77ce0adc494c2d8937068f91d7e";

    std::map<size_t, std::string> expected_passes = {
        {16, "qmRvWUccVzwsbekc"},
        {32, "sQrAnTFwOmMxzofSGhdqCcVweEKmGmxR"},
        {48, "gfvhLglwlKtuXzooqJVrpjYiAcHXxuVqHLUwNRkubGryEVTh"}
    };

    for (auto &[len, expected] : expected_passes) {
        std::vector<uint8_t> dst(len + 1, 0);
        bool result = kdf.derivatePassLettersOnly(dst.data(), len, input, seed);

        //printf("[%zu bytes]: %s\n", len, dst.data());

        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQUAL_STRING(expected.c_str(), reinterpret_cast<char*>(dst.data()));
    }
}

void test_derivatePassLettersOnly_all_output_lengths(void) {
    Kdf kdf;
    char input[] = "test";
    const char* seed = "1f517340d371cbf900369c48085c3a253821f77ce0adc494c2d8937068f91d7e";

    for (size_t len = 1; len <= 128; ++len) {
        std::vector<uint8_t> dst(len + 1, 0); // +1 for null terminator

        bool result = kdf.derivatePassLettersOnly(dst.data(), len, input, seed);

        //printf("[%3zu chars]: %s\n", len, dst.data());

        TEST_ASSERT_TRUE_MESSAGE(result, "derivatePassLettersOnly() returned false");

        size_t derived_len = std::strlen(reinterpret_cast<char*>(dst.data()));
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(len, derived_len, "Derived password length mismatch");
    }
}

/////////////////////////////
// derivatePassNumbersOnly //
/////////////////////////////

void test_derivatePassNumbersOnly_null_args() {
     Kdf kdf;
    uint8_t dst[50];
    TEST_ASSERT_FALSE(kdf.derivatePassNumbersOnly(nullptr, 10, (char*)"input", "seed"));
    TEST_ASSERT_FALSE(kdf.derivatePassNumbersOnly(dst, 10, nullptr, "seed"));
    TEST_ASSERT_FALSE(kdf.derivatePassNumbersOnly(dst, 10, (char*)"input", nullptr));
}

void test_derivatePassNumbersOnly_various_lengths() {
    Kdf kdf;
    char input[] = "test";
    const char* seed = "1f517340d371cbf900369c48085c3a253821f77ce0adc494c2d8937068f91d7e";

    std::map<size_t, std::string> expected_passes = {
        {16, "8695062891821868"},
        {32, "26741594248330126952089826082493"},
        {48, "457736943232536081911520005558323742954438700737"}
    };

    for (auto &[len, expected] : expected_passes) {
        std::vector<uint8_t> dst(len + 1, 0);
        bool result = kdf.derivatePassNumbersOnly(dst.data(), len, input, seed);

        //printf("[%zu bytes]: %s\n", len, dst.data());

        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_EQUAL_STRING(expected.c_str(), reinterpret_cast<char*>(dst.data()));
    }
}

void test_derivatePassNumbersOnly_all_output_lengths(void) {
    Kdf kdf;
    char input[] = "test";
    const char* seed = "1f517340d371cbf900369c48085c3a253821f77ce0adc494c2d8937068f91d7e";

    for (size_t len = 1; len <= 128; ++len) {
        std::vector<uint8_t> dst(len + 1, 0); // +1 for null terminator

        bool result = kdf.derivatePassNumbersOnly(dst.data(), len, input, seed);

        //printf("[%3zu chars]: %s\n", len, dst.data());

        TEST_ASSERT_TRUE_MESSAGE(result, "derivatePassNumbersOnly() returned false");

        size_t derived_len = std::strlen(reinterpret_cast<char*>(dst.data()));
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(len, derived_len, "Derived password length mismatch");
    }
}


// -----------------------------------------------------------------------------
// Test Runner
// -----------------------------------------------------------------------------
int main(int, char**) {
    UNITY_BEGIN();

    RUN_TEST(test_hkdf_various_lengths);
    RUN_TEST(test_hkdf_deterministic);

    RUN_TEST(test_derivateKey_null_args);
    RUN_TEST(test_derivateKey_various_lengths);
    RUN_TEST(test_derivateKey_all_output_lengths);

    RUN_TEST(test_derivatePass_null_args);
    RUN_TEST(test_derivatePass_various_lengths);
    RUN_TEST(test_derivatePass_all_output_lengths);

    RUN_TEST(test_derivatePassWithSymbols_null_args);
    RUN_TEST(test_derivatePassWithSymbols_various_lengths);
    RUN_TEST(test_derivatePassWithSymbols_all_output_lengths);

    RUN_TEST(test_derivatePassLettersOnly_null_args);
    RUN_TEST(test_derivatePassLettersOnly_various_lengths);
    RUN_TEST(test_derivatePassLettersOnly_all_output_lengths);

    RUN_TEST(test_derivatePassNumbersOnly_null_args);
    RUN_TEST(test_derivatePassNumbersOnly_various_lengths);
    RUN_TEST(test_derivatePassNumbersOnly_all_output_lengths);

    return UNITY_END();
}
