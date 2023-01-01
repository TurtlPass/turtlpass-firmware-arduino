#include "HexUtil.h"

void bin2hex(uint8_t* dst, uint8_t* src, size_t srcLength) {
  static const char hexChars[] = "0123456789abcdef";
  for (size_t i = 0; i < srcLength; i++) {
    uint8_t srcByte = src[i];
    dst[i * 2] = hexChars[srcByte >> 4];
    dst[i * 2 + 1] = hexChars[srcByte & 0x0f];
  }
}

int hex2bin(uint8_t *dst, uint8_t *src, size_t srcLength) {
  int i;
  for (i = 0; i < srcLength; i++) {
    uint8_t byte = src[i];
    uint8_t high_nibble = (byte & 0xF0) >> 4;
    uint8_t low_nibble = byte & 0x0F;
    if (high_nibble > 15 || low_nibble > 15) {
      return i;
    }
    dst[i] = (high_nibble << 4) + low_nibble;
  }
  return i;
}
