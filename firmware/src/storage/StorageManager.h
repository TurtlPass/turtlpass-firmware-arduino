#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <EEPROM.h>
#include <algorithm>

#define EEPROM_MAGIC 0xFA55
#define HEADER_SIZE 4  // 2 bytes magic + 2 bytes totalUsed
#define ENTRY_OVERHEAD (sizeof(uint16_t) + sizeof(uint32_t))  // len + key

/**
 * @class StorageManager
 * @brief Simple key-value storage manager for Arduino EEPROM.
 *
 * Data layout in EEPROM:
 * [0..1]   = Magic number (0xFA55)
 * [2..3]   = Total used bytes (uint16_t)
 * [4..N]   = Repeated entries of [length][key][data]
 *
 * Each entry:
 *   uint16_t length  → number of bytes in data
 *   uint32_t key     → unique 32-bit identifier
 *   uint8_t  data[]  → arbitrary binary payload
 */
class StorageManager {
public:
    StorageManager();

    /**
     * @brief Initialize the EEPROM manager and validate or reset header.
     * 
     * @param size Total EEPROM size in bytes (typically EEPROM.length()).
     */
    void begin(size_t size);

    /**
     * @brief Get total EEPROM capacity in bytes.
     * 
     * @return EEPROM size (as provided to begin()).
     */
    size_t capacity();

    /**
     * @brief Erase the entire EEPROM and reset the header.
     * 
     * Writes the magic number (0xFA55) and initializes the
     * totalUsedBytes field to the header size.
     */
    void factoryReset();

    /**
     * @brief Write a key-value pair to EEPROM.
     * 
     * Format: [length][key][data].
     * The write is rejected if the key already exists or if
     * insufficient space remains.
     *
     * @param key 32-bit unique identifier for this entry.
     * @param value Pointer to the data buffer to store.
     * @param len Length of data in bytes.
     * @return true if successful, false otherwise.
     */
    bool writeKeyValue(uint32_t key, uint8_t* value, uint16_t valueLength);

    /**
     * @brief Read the value associated with a given key.
     * 
     * Scans sequentially for the key and copies its data
     * into the provided buffer (up to expectedLen).
     *
     * @param key 32-bit identifier.
     * @param dst Destination buffer for data.
     * @param expectedLen Maximum bytes to read into dst.
     * @return true if key found and data copied successfully, false otherwise.
     */
    bool readValueByKey(uint32_t key, uint8_t* dst, uint16_t expectedLen);

    /**
     * @brief Check if a key already exists in EEPROM.
     *
     * @param key 32-bit identifier to search for.
     * @return true if found, false otherwise.
     */
    bool keyExists(uint32_t key);

    // Debug tools
    bool debugDumpKeys(uint8_t *dst, size_t maxLen, size_t &outLen);


private:
    size_t eepromSize;

    ///////////////////////////////////////////////////////////////
    // Header Utilities
    ///////////////////////////////////////////////////////////////

    /**
     * @brief Determine if header value is invalid or corrupted.
     *
     * @param headerValue Value read from header (totalUsedBytes).
     * @return true if invalid, false otherwise.
     */
    bool isHeaderInvalid(uint16_t headerValue);

    /**
     * @brief Erase all EEPROM bytes to 0xFF.
     */
    void eraseEEPROM();

    /**
     * @brief Write total used bytes field in header.
     *
     * @param value New total used bytes.
     */
    void writeTotalUsedBytes(uint16_t value);

    /**
     * @brief Read total used bytes from header.
     *
     * @return Number of bytes currently in use, including header.
     */
    uint16_t readTotalUsedBytes();

    ///////////////////////////////////////////////////////////////
    // Low-level EEPROM access helpers
    ///////////////////////////////////////////////////////////////

    /**
     * @brief Write a 16-bit unsigned integer to EEPROM.
     *
     * @param address Byte offset in EEPROM.
     * @param value 16-bit value to write.
     */
    void writeUInt16(uint16_t address, uint16_t value);

    /**
     * @brief Read a 16-bit unsigned integer from EEPROM.
     *
     * @param address Byte offset in EEPROM.
     * @return The 16-bit value read.
     */
    uint16_t readUInt16(uint16_t address);

    /**
     * @brief Write a 32-bit unsigned integer to EEPROM.
     *
     * @param address Byte offset in EEPROM.
     * @param value 32-bit value to write.
     */
    void writeUInt32(uint16_t address, uint32_t value);

    /**
     * @brief Read a 32-bit unsigned integer from EEPROM.
     *
     * @param address Byte offset in EEPROM.
     * @return The 32-bit value read.
     */
    uint32_t readUInt32(uint16_t address);

    /**
     * @brief Write a sequence of bytes to EEPROM.
     *
     * @param address Starting address.
     * @param data Source buffer.
     * @param len Number of bytes to write.
     */
    void writeBytes(uint16_t address, const uint8_t *data, uint16_t len);

    /**
     * @brief Read a sequence of bytes from EEPROM.
     *
     * @param address Starting address.
     * @param dst Destination buffer.
     * @param len Number of bytes to read.
     */
    void readBytes(uint16_t address, uint8_t *dst, uint16_t len);
};

#endif // STORAGE_MANAGER_H
