#ifndef KDF_H
#define KDF_H

#include <SoftwareSerial.h>
#include <cstdint>  // uint8_t and size_t
#include <cstdlib>
#include <cstring>
#include <cctype>
#include "SHA512.h"
#include "HKDF.h"
#include "Base62.h"
#include "Base94.hpp"

#if defined(__TURTLPASS_PASS_SIZE__)
#define PASS_SIZE __TURTLPASS_PASS_SIZE__
#else
#define PASS_SIZE 100  // 100 characters
#endif

const size_t BITS_PER_BYTE = 8;
const size_t BASE62_ENCODED_BITS = 6;
const size_t BASE94_INPUT_BLOCK_SIZE = 9;
const size_t BASE94_OUTPUT_BLOCK_SIZE = 11;

class Kdf {
public:
  bool derivatePass(uint8_t *dst, size_t dstLength, char *input, const char *seed);
  bool derivatePassWithSymbols(uint8_t *dst, size_t dstLength, char *input, const char *seed);
  bool derivateKey(uint8_t *dst, size_t dstLength, char *input, const char *seed);

private:
  void hkdf(uint8_t *dst, size_t dstLength, const uint8_t *src, size_t srcLength, const uint8_t *salt, size_t saltLength);
  static uint32_t base62InputLength(size_t encodedLength);
  static uint32_t base94InputLength(size_t encodedLength);
};

#endif  // KDF_H
