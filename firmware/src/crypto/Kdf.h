#ifndef KDF_H
#define KDF_H

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cmath>
#include <functional>
#include "SHA512.h"
#include "HKDF.h"
#include "Base62.h"
#include "Base94.hpp"


/**
 * @class Kdf
 * @brief Key derivation and password generation utility with multiple encoding modes.
 *
 * The `Kdf` class provides deterministic methods to derive cryptographic keys or
 * human-readable passwords from an input string and a seed. It supports several
 * output encodings for different use cases:
 *   - **Base62**: Alphanumeric (A–Z, a–z, 0–9). Safe for general use and URLs.
 *   - **Base94**: All printable ASCII characters. Maximizes entropy per character.
 *   - **Base52**: Letters only (A–Z, a–z). Useful when symbols or digits are disallowed.
 *   - **Base10**: Digits only (0–9). Suitable for numeric-only systems or PINs.
 *
 * Internally, all derivation functions:
 *   - Append the destination length to the input before key derivation (to ensure uniqueness per length).
 *   - Use an HKDF-based key derivation with a provided seed.
 *   - Encode the derived key using the specified base or character set.
 *
 * This design ensures that for a given (input, seed, output length, encoding),
 * the same password or key is always produced — providing deterministic,
 * secure, and flexible derivation for both machine and human use.
 *
 * @note
 * The encoding helpers (`baseXXInputLength()` and `encodeXX()`) are used
 * internally by the main derivation methods and are not intended for direct use.
 *
 * @see derivatePass()
 * @see derivatePassWithSymbols()
 * @see derivatePassLettersOnly()
 * @see derivatePassNumbersOnly()
 */
class Kdf {
public:
  /**
   * @brief Derive a binary key from input data and a seed.
   *
   * This function produces a deterministic key of the requested size. The key
   * derivation algorithm ensures that the same input and seed always produce
   * the same output.
   *
   * @param dst Pointer to the output buffer.
   * @param dstLen Length of the output buffer in bytes.
   * @param input Input string to derive the key from.
   * @param seed Seed string for additional entropy.
   * @return true if the derivation succeeded, false otherwise (e.g., null pointers).
   */
  bool derivateKey(uint8_t *dst, size_t dstLength, char *input, const char *seed);

   /**
   * @brief Derive a password string from input and seed.
   *
   * The derived password is encoded in Base62 (alphanumeric) and fits in the
   * provided buffer. Base62 is chosen because it produces passwords that are:
   *   - Alphanumeric only, making them safe for URLs, databases, shells, and user input.
   *   - Compact, efficiently representing high-entropy keys.
   *   - Easy to type, read, and copy without confusion.
   *   - Compatible with most systems without requiring escaping or special handling.
   *
   * @param dst Pointer to the output buffer (must be at least dstLength+1 for null-terminator).
   * @param dstLength Desired length of the password.
   * @param input Null-terminated input string (e.g., a password).
   * @param seed Null-terminated seed string.
   * @return true if the password was successfully derived and encoded, false otherwise.
   */
  bool derivatePass(uint8_t *dst, size_t dstLength, const char *input, const char *seed);

  /**
   * @brief Derive a password string from input and seed, using extended symbols.
   *
   * The derived password is encoded in Base94 (all printable ASCII characters)
   * and fits in the provided buffer. Base94 is chosen because it produces passwords that:
   *   - Include all printable ASCII characters (letters, digits, symbols), increasing entropy per character.
   *   - Are suitable when a stronger, more complex password is desired.
   *   - Retain full cryptographic entropy from the derived key.
   *
   * Note that Base94 passwords may include characters that require escaping in
   * URLs, shells, or certain databases, and may be harder for users to type
   * compared to Base62.
   *
   * @param dst Pointer to the output buffer (must be at least dstLength+1 for null-terminator).
   * @param dstLength Desired length of the password.
   * @param input Null-terminated input string (e.g., a password).
   * @param seed Null-terminated seed string.
   * @return true if the password was successfully derived and encoded, false otherwise.
   */
  bool derivatePassWithSymbols(uint8_t *dst, size_t dstLength, const char *input, const char *seed);

  /**
   * @brief Derive a password string from input and seed, using letters only.
   *
   * The derived password is encoded using only letters (a-z, A-Z) and fits in
   * the provided buffer. This “letters-only” encoding is chosen because it:
   *   - Produces passwords that are easy to type and remember.
   *   - Avoids digits and symbols, reducing the risk of input errors on some
   *     devices or systems.
   *   - Maintains a reasonable level of entropy, suitable for applications
   *     that don’t require symbols or numbers.
   *
   * Note that each byte of the derived key is mapped modulo 52 to select a letter.
   *
   * @param dst Pointer to the output buffer (must be at least dstLength+1 for null-terminator).
   * @param dstLength Desired length of the password.
   * @param input Null-terminated input string (e.g., a password).
   * @param seed Null-terminated seed string.
   * @return true if the password was successfully derived and encoded, false otherwise.
   */
  bool derivatePassLettersOnly(uint8_t *dst, size_t dstLength, const char *input, const char *seed);

  /**
   * @brief Derive a password string from input and seed, using digits only.
   *
   * The derived password is encoded using digits (0–9) and fits in the provided buffer.
   * This “numbers-only” encoding is chosen because it:
   *   - Produces numeric passwords (PINs) that are easy to enter on devices with limited input options.
   *   - Avoids letters and symbols, making it suitable for numeric-only systems or user interfaces.
   *   - Maintains deterministic derivation with reasonable entropy for its length.
   *
   * Each byte of the derived key is mapped modulo 10 to select a digit.
   *
   * @param dst Pointer to the output buffer (must be at least dstLength + 1 for null-terminator).
   * @param dstLength Desired length of the password.
   * @param input Null-terminated input string (e.g., a password).
   * @param seed Null-terminated seed string.
   * @return true if the password was successfully derived and encoded, false otherwise.
   *
   * @note This function uses the internal deriveAndEncode() helper, similar to
   *       derivatePass(), derivatePassWithSymbols(), and derivatePassLettersOnly(),
   *       but restricts the output to numeric characters only.
   */
  bool derivatePassNumbersOnly(uint8_t *dst, size_t dstLength, const char *input, const char *seed);


private:
  static constexpr size_t BITS_PER_BYTE = 8;
  static constexpr size_t BASE62_ENCODED_BITS = 6;
  static constexpr size_t BASE94_INPUT_BLOCK_SIZE = 9;
  static constexpr size_t BASE94_OUTPUT_BLOCK_SIZE = 11;

  /**
   * @brief Derives a cryptographic key from the input and encodes it.
   *
   * This is a shared helper function used by different password derivation
   * methods (e.g., Base62, Base94). It performs the following steps:
   *   1. Validates input pointers.
   *   2. Appends the textual value of `dstLength` to the input string.
   *   3. Allocates a buffer for the intermediate key based on the desired
   *      output length using `keyLengthFunc`.
   *   4. Calls `derivateKey()` to derive the key using the modified input
   *      and the provided seed.
   *   5. Encodes the derived key using the provided encoding function
   *      (`encodeFunc`).
   *   6. Copies the encoded key to the destination buffer (`dst`) and
   *      ensures null-termination.
   *
   * @param dst Pointer to the buffer where the final encoded key will be stored.
   *            Must be at least `dstLength + 1` bytes.
   * @param dstLength Length of the destination buffer (`dst`). The derived
   *                  key will be truncated if necessary.
   * @param input Null-terminated input string (e.g., a password).
   * @param seed Null-terminated seed string used for key derivation.
   * @param keyLengthFunc Function pointer to compute the intermediate key length
   *                      from the desired encoded length (e.g., `base62InputLength`
   *                      or `base94InputLength`). Must return `size_t`.
   * @param encodeFunc Function pointer or lambda that encodes the intermediate key
   *                   into a string. Signature: `bool encodeFunc(const std::vector<uint8_t>& key, std::string& out)`.
   *                   Returns true if encoding succeeds.
   *
   * @return true if the key was successfully derived and encoded, false otherwise.
   *
   * @note This function is intended to be used internally as a private helper.
   *       It does not perform any memory allocation for the destination buffer;
   *       `dst` must be pre-allocated.
   * @note The function appends `dstLength` to the input string before deriving
   *       the key to ensure uniqueness of the derived key for different lengths.
   */
  bool deriveAndEncode(
      uint8_t *dst,
      size_t dstLength,
      const char *input,
      const char *seed,
      size_t (*keyLengthFunc)(size_t),
      std::function<bool(const std::vector<uint8_t>&, std::string&)> encodeFunc);

  /**
   * @brief Perform HKDF (HMAC-based Key Derivation Function) to derive key material.
   *
   * This function implements the HKDF algorithm, which extracts and expands key material
   * from an input key (`src`) using a salt. The output is a deterministic byte sequence
   * of length `dstLength`.
   *
   * HKDF is widely used in cryptography to generate keys from shared secrets, passwords,
   * or other entropy sources, ensuring uniform distribution and unpredictability.
   *
   * @param dst Pointer to the output buffer where derived key bytes will be written.
   *            Must be at least `dstLength` bytes.
   * @param dstLength Number of bytes to generate for the derived key.
   * @param src Pointer to the input key material (secret) used as the source.
   * @param srcLength Length of the input key material in bytes.
   * @param salt Pointer to the salt value (can be nullptr or empty for a default salt).
   * @param saltLength Length of the salt in bytes (0 if no salt is used).
   *
   * @note The function is deterministic: the same `src` and `salt` always produce the same output.
   * @note Make sure `dst` buffer is allocated and large enough to hold `dstLength` bytes.
   */
  void hkdf(uint8_t *dst, size_t dstLength, const uint8_t *src, size_t srcLength, const uint8_t *salt, size_t saltLength);
  
  ////////////////////////////////////////////////////////
  // Base62 / Base94 / Base52 / Base10 encoding helpers //
  ////////////////////////////////////////////////////////

  /**
   * @brief Compute the required intermediate key length for Base62 encoding.
   *
   * This function calculates how many bytes of intermediate key material are
   * needed to produce a Base62-encoded password of the requested length.
   * 
   * Base62 encoding expands raw bytes into characters (roughly 1 byte → 1.367 characters),
   * so to guarantee that the final encoded string is at least `dstLength` characters,
   * the returned key length is computed using the logarithmic expansion ratio and
   * rounded up.
   *
   * A minimum of 2 bytes is returned to ensure that even very short passwords
   * (e.g., dstLength = 1) have sufficient entropy for encoding.
   *
   * @param dstLength Desired length of the final Base62 password.
   * @return Number of bytes to generate with the key derivation function.
   *
   * @note The derived key produced with this length is later Base62-encoded
   *       and truncated or padded to exactly `dstLength` characters.
   */
  static size_t base62InputLength(size_t encodedLength);

  /**
   * @brief Calculate the input length needed for a Base94-encoded output.
   *
   * Base94 encoding works in blocks. Each output block has size BASE94_OUTPUT_BLOCK_SIZE
   * and corresponds to an input block of size BASE94_INPUT_BLOCK_SIZE.
   *
   * @param encodedLength Desired length of the Base94-encoded string.
   * @return Total input length in bytes required to produce the encoded output.
   *
   * @note Rounds up if encodedLength is not a multiple of BASE94_OUTPUT_BLOCK_SIZE.
   */
  static size_t base94InputLength(size_t encodedLength);

  /**
   * @brief Compute the required intermediate key length for a letters-only password.
   *
   * This function calculates the number of bytes needed for the intermediate
   * key to produce a derived password of the requested length. It is designed
   * for use with letters-only (a-z, A-Z) encoding in derivatePassLettersOnly().
   *
   * @param dstLength Desired length of the final password.
   * @return The required length of the intermediate key in bytes.
   *
   * @note Each byte of the intermediate key will be mapped modulo 52 to a
   *       letter. For simplicity, the returned value is equal to dstLength.
   *       This ensures a 1:1 mapping from key bytes to letters.
   */
  static size_t base52InputLength(size_t encodedLength);

  /**
   * @brief Compute the required intermediate key length for a numbers-only password.
   *
   * This function determines the number of bytes needed for the intermediate key
   * to produce a derived password of the requested length. It is designed for use
   * with numeric (0–9) encoding in derivatePassNumbersOnly().
   *
   * @param dstLength Desired length of the final password.
   * @return The required length of the intermediate key in bytes.
   *
   * @note Each byte of the intermediate key is mapped modulo 10 to a single digit.
   *       For simplicity, the returned value is equal to dstLength, ensuring a 1:1
   *       mapping between key bytes and output digits.
   */
  static size_t base10InputLength(size_t encodedLength);

  /**
   * @brief Encode an intermediate key into a letters-only string.
   *
   * This function maps each byte of the derived key to a letter in the
   * range a-z, A-Z. It is intended for use with derivatePassLettersOnly().
   *
   * @param key Reference to the vector containing the derived key bytes.
   * @param out Reference to a std::string where the encoded password will be stored.
   * @return true if encoding succeeds, false otherwise.
   *
   * @note Each byte is mapped modulo 52 to select a letter from the
   *       52-character set. The output string is cleared before encoding.
   * @note This function preserves the entropy of the derived key within the
   *       constraints of the letters-only encoding.
   */
  static bool encodeLettersOnly(const std::vector<uint8_t>& key, std::string& out);

  /**
   * @brief Encode an intermediate key into a numeric string.
   *
   * This function maps each byte of the derived key to a digit (0–9). It is used
   * internally by derivatePassNumbersOnly() to produce numeric-only passwords or PINs.
   *
   * @param key Reference to the vector containing the derived key bytes.
   * @param out Reference to a std::string where the encoded numeric password will be stored.
   * @return true if encoding succeeds, false otherwise.
   *
   * @note Each byte is mapped modulo 10 to select a digit from the 10-character set.
   *       The output string is cleared before encoding.
   * @note This encoding preserves determinism while restricting the output space
   *       to numeric characters, making it suitable for PIN-style passwords or
   *       systems that accept digits only.
   */
  static bool encodeNumbersOnly(const std::vector<uint8_t>& key, std::string& out);
};

#endif  // KDF_H
