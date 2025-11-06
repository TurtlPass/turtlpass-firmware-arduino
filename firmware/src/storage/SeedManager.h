#ifndef SEED_MANAGER_H
#define SEED_MANAGER_H

#include "storage/StorageManager.h"
#include "crypto/EncryptionManager.h"
#include <stdint.h>
#include <stddef.h>

#if defined(TP_EEPROM_SIZE)
#define EEPROM_SIZE TP_EEPROM_SIZE
#else
#define EEPROM_SIZE 4096
#endif

/**
 * @class SeedManager
 * @brief Manages storage, encryption, and retrieval of seeds in EEPROM.
 *
 * Handles:
 *  - Storing a new seed securely in slots 1–9.
 *  - Retrieving stored seeds and decrypting them.
 *  - Verifying data integrity.
 *  - Factory reset of all seeds.
 */
class SeedManager {
public:
    // Size of each seed in bytes
    static const size_t SEED_SIZE = SHA512::HASH_SIZE;

    // Maximum number of slots available
    static const size_t NUM_SLOTS = 9;

    /**
     * @enum SeedInitResult
     * @brief Result codes for seed initialization
     */
    enum class SeedInitResult : uint8_t {
        OK = 0,                // Seed successfully stored and verified
        INVALID_SLOT,          // Slot number is invalid (not 1–NUM_SLOTS)
        INVALID_INPUT,         // Input seed is null or wrong length
        ALREADY_POPULATED,     // Slot already contains a seed
        WRITE_FAIL,            // Failed to write encrypted seed to EEPROM
        READ_FAIL,             // Failed to read encrypted seed back from EEPROM
        VERIFY_FAIL            // Seed verification failed after write
    };

    /**
     * @brief Constructor. Does not initialize storage.
     */
    SeedManager();

    /**
     * @brief Initializes the storage manager backend.
     */
    void begin();

    /**
     * @brief Initializes a new seed in the specified slot.
     *
     * Performs validation, hashes the seed, encrypts it, stores it, reads back for
     * verification, and decrypts to confirm integrity.
     *
     * @param seedSlot Slot number (1–NUM_SLOTS) to store the seed.
     * @param seed Pointer to the seed bytes to store.
     * @param seedLen Length of the seed (must equal SEED_SIZE).
     * @return SeedInitResult Enum indicating success or failure reason.
     */
    SeedInitResult initializeSeed(uint8_t seedSlot, const uint8_t* seed, size_t seedLen);

    /**
     * @brief Retrieves a stored seed from a slot.
     *
     * @param seedSlot Slot number (1–NUM_SLOTS) to retrieve the seed from.
     * @param seedOut Output buffer to store the decrypted seed (must be at least SEED_SIZE bytes).
     * @param seedLen Length of the output buffer.
     * @return true if successful, false otherwise (invalid slot, missing seed, or decryption failure).
     */
    bool getSeed(uint8_t seedSlot, uint8_t* seedOut, size_t seedLen);

    /**
     * @brief Factory reset: clears all stored seeds and reinitializes storage.
     */
    void factoryReset();

private:
    StorageManager storageManager;  // Handles low-level EEPROM read/write
    EncryptionManager encryption;   // Handles seed encryption and decryption
};

#endif
