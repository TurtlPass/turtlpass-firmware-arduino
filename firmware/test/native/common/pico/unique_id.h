#ifndef PICO_UNIQUE_ID_H
#define PICO_UNIQUE_ID_H

#include <stdint.h>
#include <string.h>

#define PICO_UNIQUE_BOARD_ID_SIZE_BYTES 8

typedef struct {
    uint8_t id[PICO_UNIQUE_BOARD_ID_SIZE_BYTES];
} pico_unique_board_id_t;

inline void pico_get_unique_board_id(pico_unique_board_id_t* id) {
    for (size_t i = 0; i < PICO_UNIQUE_BOARD_ID_SIZE_BYTES; ++i)
        id->id[i] = (uint8_t)(0xA0 + i); // deterministic fake ID
}

#endif  // PICO_UNIQUE_ID_H
