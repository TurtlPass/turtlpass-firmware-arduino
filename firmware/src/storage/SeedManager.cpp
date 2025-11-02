#include "SeedManager.h"

SeedManager::SeedManager() {}

void SeedManager::begin() {
    storageManager.begin(EEPROM_SIZE);
}

SeedManager::SeedInitResult SeedManager::initializeSeed(uint8_t seedSlot, const uint8_t* seedInput, size_t seedLen) {
    // --- Validate input ---
    if (!seedInput || seedLen != SEED_SIZE) return SeedInitResult::INVALID_INPUT; // Must match SEED_SIZE
    if (seedSlot == 0 || seedSlot > NUM_SLOTS) return SeedInitResult::INVALID_SLOT; // Only slots 1–9

    // --- Check if slot is already populated ---
    uint8_t existing[SEED_SIZE] = {0};
    if (getSeed(seedSlot, existing, SEED_SIZE)) {
        memset(existing, 0, sizeof(existing));
        return SeedInitResult::ALREADY_POPULATED;
    }
    memset(existing, 0, sizeof(existing));

    // --- Hash the input seed ---
    uint8_t seed[SHA512::HASH_SIZE] = {0};
    SHA512 sha;
    sha.reset();
    sha.update(seedInput, seedLen);
    sha.finalize(seed, sizeof(seed));

    // --- Encrypt the seed ---
    encryption.init(seedSlot); // Initialize encryption for this slot
    uint8_t ciphertext[SEED_SIZE] = {0};
    if (!encryption.encrypt(ciphertext, seed, sizeof(seed))) {
        memset(ciphertext, 0, sizeof(ciphertext));
        memset(seed, 0, sizeof(seed));
        return SeedInitResult::WRITE_FAIL;
    }

    // --- Write the ciphertext to EEPROM ---
    if (!storageManager.writeKeyValue(seedSlot, ciphertext, (uint16_t)sizeof(seed))) {
        memset(ciphertext, 0, sizeof(ciphertext));
        memset(seed, 0, sizeof(seed));
        return SeedInitResult::WRITE_FAIL;
    }

    // --- Read back ciphertext for verification ---
    uint8_t readback[SEED_SIZE] = {0};
    if (!storageManager.readValueByKey(seedSlot, readback, (uint16_t)sizeof(seed))) {
        memset(ciphertext, 0, sizeof(ciphertext));
        memset(readback, 0, sizeof(readback));
        memset(seed, 0, sizeof(seed));
        return SeedInitResult::READ_FAIL;
    }

    // --- Verify written data matches readback ---
    if (memcmp(ciphertext, readback, sizeof(seed)) != 0) {
        memset(ciphertext, 0, sizeof(ciphertext));
        memset(readback, 0, sizeof(readback));
        memset(seed, 0, sizeof(seed));
        return SeedInitResult::WRITE_FAIL;
    }

    // --- Decrypt the stored ciphertext and verify against original ---
    encryption.init(seedSlot);
    uint8_t decrypted[SEED_SIZE] = {0};
    if (!encryption.decrypt(decrypted, readback, sizeof(seed))) {
        memset(ciphertext, 0, sizeof(ciphertext));
        memset(readback, 0, sizeof(readback));
        memset(decrypted, 0, sizeof(decrypted));
        memset(seed, 0, sizeof(seed));
        return SeedInitResult::VERIFY_FAIL;
    }
    if (memcmp(decrypted, seed, sizeof(seed)) != 0) {
        memset(ciphertext, 0, sizeof(ciphertext));
        memset(readback, 0, sizeof(readback));
        memset(decrypted, 0, sizeof(decrypted));
        memset(seed, 0, sizeof(seed));
        return SeedInitResult::VERIFY_FAIL;
    }

    // --- Success: zero sensitive buffers before returning ---
    memset(ciphertext, 0, sizeof(ciphertext));
    memset(readback, 0, sizeof(readback));
    memset(decrypted, 0, sizeof(decrypted));
    memset(seed, 0, sizeof(seed));

    return SeedInitResult::OK;
}

bool SeedManager::getSeed(uint8_t seedSlot, uint8_t* seedOut, size_t seedLen) {
    if (!seedOut || seedLen < SEED_SIZE) return false; // Output must be valid and large enough
    if (seedSlot == 0 || seedSlot > NUM_SLOTS) return false;

    // Read ciphertext from storage
    uint8_t encrypted[SEED_SIZE] = {0};
    if (!storageManager.readValueByKey(seedSlot, encrypted, SEED_SIZE)) {
        memset(encrypted, 0, sizeof(encrypted));
        return false;
    }

    // Treat all-0xFF as empty (erased) slot
    bool isEmpty = true;
    for (size_t i = 0; i < SEED_SIZE; ++i) {
        if (encrypted[i] != 0xFF) {
            isEmpty = false;
            break;
        }
    }
    if (isEmpty) return false;

    // Decrypt to output buffer
    encryption.init(seedSlot);
    bool ok = encryption.decrypt(seedOut, encrypted, SEED_SIZE);

    // Zero sensitive buffer
    memset(encrypted, 0, sizeof(encrypted));

    return ok;
}

void SeedManager::factoryReset() {
    storageManager.factoryReset();
    storageManager.begin(storageManager.capacity()); // Re-initialize storage
}
