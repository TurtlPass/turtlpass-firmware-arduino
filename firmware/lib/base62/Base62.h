// source: https://github.com/ericherman/base62
/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* Base62.h: a base62 encoder */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#ifndef BASE62_H
#define BASE62_H

#include <stddef.h>
#include <stdint.h>

extern const char *base62_charset;

char *base62_encode(char *buf, size_t buf_len,
                    const uint8_t *data, size_t data_len);

uint8_t *base62_decode(uint8_t *buf, size_t buf_len, size_t *written,
                       const char *encoded, size_t encoded_len);

#endif
