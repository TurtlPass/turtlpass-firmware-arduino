#ifndef HEX_UTIL_H
#define HEX_UTIL_H

#include <Arduino.h>

void bin2hex(uint8_t* dst, uint8_t* src, size_t srcLength);
int hex2bin(uint8_t *dst, uint8_t *src, size_t srcLength);

#endif // HEX_UTIL_H
