#ifndef KDF_H
#define KDF_H

#include <SoftwareSerial.h>
#include <cstdint>  // uint8_t and size_t
#include <cstdlib>
#include <cstring>
#include <cctype>
#include "SHA512.h"
#include "HKDF.h"

#if defined(__TURTLPASS_PASS_SIZE__)
#define PASS_SIZE __TURTLPASS_PASS_SIZE__
#else
#define PASS_SIZE 100 // 100 characters
#endif

class Kdf {
public:
  bool derivatePass(uint8_t *dst, size_t dstLength, char *input, const char *seed);
  bool derivateKey(uint8_t *dst, size_t dstLength, char *input, const char *seed);

private:
  void hkdf(uint8_t *dst, size_t dstLength, const uint8_t *src, size_t srcLength, const uint8_t *salt, size_t saltLength);
  static void key2Charset(uint8_t *dst, const uint8_t *src, size_t srcLength);
  static size_t base62InputLength(size_t encodedLength);
};

#endif  // KDF_H
