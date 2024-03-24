/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* Base62.cpp: a base62 encoder inspired by John Jiyang Hou's java version */
/* as can bee seen: https://www.codeproject.com/articles/1076295/base-encode */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include "Base62.h"

#ifndef Eprintf
#ifdef NDEBUG
#define Eprintf(fmt, ...) \
  do { (void)0; } while (0)
#endif
#endif

#ifndef Eprintf
#include <stdio.h>
#define Eprintf(fmt, ...) \
  do { \
    fflush(stdout); \
    fprintf(stderr, "%s:%d: ", __FILE__, __LINE__); \
    fprintf(stderr, fmt, __VA_ARGS__); \
    fprintf(stderr, "\n"); \
  } while (0)
#endif

#ifndef base62_strlen
#include <string.h>
#define base62_strlen(s) strlen(s)
#endif

#ifndef base62_memset
#include <string.h>
#define base62_memset(s, v, n) memset(s, v, n)
#endif

#ifndef base62_isspace
#include <ctype.h>
#define base62_isspace(c) isspace(c)
#endif

const char *base62_charset =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

static size_t base62_str_index_of(const char *haystack, size_t haystack_len,
                                  char needle) {
  if (!haystack || needle == '\0') {
    return SIZE_MAX;
  }

  for (size_t i = 0; i < haystack_len; ++i) {
    if (haystack[i] == needle) {
      return i;
    }
  }
  return SIZE_MAX;
}

static size_t base62_index(char c) {
  size_t len = base62_strlen(base62_charset);
  return base62_str_index_of(base62_charset, len, c);
}

static size_t base62_append(char *buf, size_t buflen, size_t b) {
  size_t p = base62_strlen(buf);
  if ((p + 2) >= buflen) {
    return 0;
  }
  if (b < 61) {
    buf[p] = base62_charset[b];
    return 1;
  }
  if ((p + 3) >= buflen) {
    return 0;
  }
  if (b > 63) {
    return 0;
  }
  buf[p] = base62_charset[61];
  buf[p + 1] = base62_charset[b - 61];
  return 2;
}

char *base62_encode(char *buf, size_t buf_len,
                    const uint8_t *data, size_t data_len) {
  if (!buf) {
    return NULL;
  }
  base62_memset(buf, 0x00, buf_len);

  size_t enough_space = 1;
  for (size_t i = 0; enough_space && i < data_len; i += 3) {
    uint32_t v24 = 0;
    v24 = ((data[i] << 16)
           | ((i + 1 < data_len) ? data[i + 1] << 8 : 0)
           | ((i + 2 < data_len) ? data[i + 2] : 0));

    enough_space = base62_append(buf, buf_len, (v24 >> 18) & 0x3F);
    if (enough_space) {
      enough_space =
        base62_append(buf, buf_len, (v24 >> 12) & 0x3F);
    }
    if (enough_space && (i + 1 < data_len)) {
      enough_space =
        base62_append(buf, buf_len, (v24 >> 6) & 0x3F);
    }
    if (enough_space && (i + 2 < data_len)) {
      enough_space = base62_append(buf, buf_len, v24 & 0x3F);
    }
  }

  return enough_space ? buf : NULL;
}

uint8_t *base62_decode(uint8_t *buf, size_t buf_len, size_t *written,
                       const char *encoded, size_t encoded_len) {
  if (written) {
    *written = 0;
  }
  if (!buf || !buf_len || !written) {
    Eprintf("%p, %zu, %p?", (void *)buf, buf_len, (void *)written);
    return NULL;
  }
  base62_memset(buf, 0x00, buf_len);

  uint8_t word[4];

  (*written) = 0;
  size_t j = 0;
  for (size_t i = 0; i < encoded_len && encoded[i]; ++i) {
    if (base62_isspace(encoded[i])) {
      continue;
    }

    char c = encoded[i];
    if (c != base62_charset[61]) {
      size_t index = base62_index(c);
      if (index == SIZE_MAX) {
        Eprintf("encoded[%zu] = '%c'?", encoded_len, c);
        return NULL;
      }
      word[j++] = index;
    } else {
      ++i;
      if (i >= encoded_len) {
        Eprintf("%zu >= %zu?", i, encoded_len);
        return NULL;
      }
      c = encoded[i];
      switch (c) {
        case 'A':
          word[j++] = 61;
          break;
        case 'B':
          word[j++] = 62;
          break;
        case 'C':
          word[j++] = 63;
          break;
        default:
          Eprintf("c: '%c'?", c);
          return NULL;
      }
    }

    if (j == 4) {
      if ((*written) + 2 >= buf_len) {
        Eprintf("%zu + 2 >= %zu?", (*written), buf_len);
        return NULL;
      }
      buf[(*written)++] = ((word[0] << 2) | (word[1] >> 4));
      buf[(*written)++] = ((word[1] << 4) | (word[2] >> 2));
      buf[(*written)++] = ((word[2] << 6) | (word[3] >> 0));
      j = 0;
    }
  }

  if (j && (*written) >= buf_len) {
    Eprintf("%zu >= %zu?", (*written), buf_len);
    return NULL;
  }

  switch (j) {
    case 0:
      break;
    case 1:
      buf[(*written)++] = (word[0] << 2);
      break;
    case 2:
      buf[(*written)++] = (word[0] << 2) | (word[1] >> 4);
      break;
    case 3:
      buf[(*written)++] = ((word[0] << 2) | (word[1] >> 4));
      if ((*written) >= buf_len) {
        Eprintf("%zu + 1 >= %zu?", (*written), buf_len);
        return NULL;
      }
      buf[(*written)++] = ((word[1] << 4) | (word[2] >> 2));
      break;
    default:
      Eprintf("%zu > 3?", j);
      return NULL;
  }

  return buf;
}
