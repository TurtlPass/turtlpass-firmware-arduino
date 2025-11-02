#include "crypto/EncryptionManager.h"


EncryptionManager::EncryptionManager() : initializedSlot(-1) {
    memset(encryptionKey, 0, sizeof(encryptionKey));
    memset(encryptionIV, 0, sizeof(encryptionIV));
}

void EncryptionManager::init(uint8_t seedSlot) {
    // wipe previous key/IV and slot
    clear();  

    // Get unique hardware ID (binary 8 bytes)
    pico_unique_board_id_t unique_id;
    pico_get_unique_board_id(&unique_id);

    // Convert binary ID: hex string for deterministic KDF seed
    char boardIdHex[17]; // 8 bytes = 16 hex chars + null terminator
    for (int i = 0; i < 8; ++i)
        snprintf(&boardIdHex[i * 2], 3, "%02X", unique_id.id[i]);
    boardIdHex[16] = '\0';

    // Derive encryption key
    char keyContext[32];
    snprintf(keyContext, sizeof(keyContext), "key_slot_%u", seedSlot);
    kdf.derivateKey(encryptionKey, ENCRYPTION_KEY_SIZE, keyContext, boardIdHex);

    // Derive deterministic IV per seed slot
    char ivContext[32];
    snprintf(ivContext, sizeof(ivContext), "iv_slot_%u", seedSlot);
    kdf.derivateKey(encryptionIV, ENCRYPTION_IV_SIZE, ivContext, boardIdHex);

    // store the slot
    initializedSlot = seedSlot;
}

bool EncryptionManager::isInitialized() {
    return initializedSlot != -1 && 
        !isAllZero(encryptionKey, ENCRYPTION_KEY_SIZE) &&
        !isAllZero(encryptionIV, ENCRYPTION_IV_SIZE);
}

bool EncryptionManager::encrypt(uint8_t* dst, const uint8_t* src, size_t length) {
    if (!isInitialized() || !dst || !src || length == 0) return false;
    chachapoly.clear();
    chachapoly.setKey(encryptionKey, ENCRYPTION_KEY_SIZE);
    chachapoly.setIV(encryptionIV, ENCRYPTION_IV_SIZE);
    chachapoly.encrypt(dst, src, length);
    return true;
}

bool EncryptionManager::decrypt(uint8_t* dst, uint8_t* src, size_t length) {
    if (!isInitialized() || !dst || !src || length == 0) return false;
    chachapoly.clear();
    chachapoly.setKey(encryptionKey, ENCRYPTION_KEY_SIZE);
    chachapoly.setIV(encryptionIV, ENCRYPTION_IV_SIZE);
    chachapoly.decrypt(dst, src, length);
    return true;
}

void EncryptionManager::clear() {
    // Clear cipher state and wipe key/IV
    chachapoly.clear();
    memset(encryptionKey, 0, sizeof(encryptionKey));
    memset(encryptionIV, 0, sizeof(encryptionIV));
    initializedSlot = -1;
}

// Helper function
bool EncryptionManager::isAllZero(const uint8_t* data, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        if (data[i] != 0) return false;
    }
    return true;
}