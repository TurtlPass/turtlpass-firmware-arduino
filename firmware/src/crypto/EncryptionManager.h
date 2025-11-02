#ifndef ENCRYPTION_MANAGER_H
#define ENCRYPTION_MANAGER_H

#include <stdlib.h>
#include <stdint.h>
#include <cstring>
#include "ChaChaPoly.h"
#include "Crypto.h"
#include "Kdf.h"
#include "pico/unique_id.h"

#define ENCRYPTION_KEY_SIZE 32  ///< 32 bytes = 256-bit encryption key
#define ENCRYPTION_IV_SIZE 12   ///< 12 bytes = 96-bit IV (nonce)


/**
 * @class EncryptionManager
 * @brief Handles deterministic per-slot encryption and decryption using ChaCha20-Poly1305.
 *
 * Features:
 *  - Slot-based deterministic key and IV derivation via KDF.
 *  - Encrypt/decrypt arbitrary byte arrays.
 *  - Secure clearing of key and IV when resetting or re-initializing.
 *
 * Notes:
 *  - Uses Pico unique board ID for deterministic key derivation.
 *  - Intended for use with SeedManager to securely store slot-based secrets.
 */
class EncryptionManager {
public:
    /**
     * @brief Constructor. Initializes internal state to "uninitialized".
     */
    EncryptionManager();

    /**
     * @brief Initialize encryption for a specific seed slot.
     *
     * Derives a deterministic key and IV for the slot using the board's unique ID.
     * Clears any previous key/IV before derivation.
     *
     * @param seedSlot Slot number (used in context string for deterministic KDF derivation)
     */
    void init(uint8_t seedSlot);

    /**
     * @brief Check whether the manager is properly initialized.
     *
     * Returns true if both the key and IV are non-zero and a slot has been set.
     *
     * @return true if initialized, false otherwise
     */
    bool isInitialized();

    /**
     * @brief Encrypt a buffer using the current key and IV.
     *
     * @param dst Destination buffer (must be at least `length` bytes)
     * @param src Source buffer to encrypt
     * @param length Number of bytes to encrypt
     * @return true on success, false if not initialized or invalid arguments
     */
    bool encrypt(uint8_t* dst, const uint8_t* src, size_t length);

    /**
     * @brief Decrypt a buffer using the current key and IV.
     *
     * @param dst Destination buffer (must be at least `length` bytes)
     * @param src Source buffer to decrypt
     * @param length Number of bytes to decrypt
     * @return true on success, false if not initialized or invalid arguments
     */
    bool decrypt(uint8_t* dst, uint8_t* src, size_t length);

    /**
     * @brief Clear internal state.
     *
     * Wipes the encryption key and IV, clears the ChaChaPoly state, and marks the manager as uninitialized.
     */
    void clear();

private:
    /**
     * @brief Check if a data buffer contains only zero bytes.
     *
     * @param data Pointer to the buffer
     * @param len Length of the buffer
     * @return true if all bytes are zero, false otherwise
     */
    bool isAllZero(const uint8_t* data, size_t len);

    Kdf kdf;                                    ///< Key derivation helper
    ChaChaPoly chachapoly;                      ///< ChaCha20-Poly1305 cipher
    uint8_t encryptionKey[ENCRYPTION_KEY_SIZE]; ///< Derived encryption key
    uint8_t encryptionIV[ENCRYPTION_IV_SIZE];   ///< Derived IV / nonce
    int initializedSlot;                        ///< Slot currently initialized (-1 if none)
};


#endif  // ENCRYPTION_MANAGER_H
