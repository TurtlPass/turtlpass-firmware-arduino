#include "storage/StorageManager.h"

///////////////////////////////////////////////////////////////
// Constructor & Initialization
///////////////////////////////////////////////////////////////

StorageManager::StorageManager() {}

void StorageManager::begin(size_t size) {
    eepromSize = size; 
    EEPROM.begin(size);

    uint16_t magic = readUInt16(0);
    uint16_t totalUsed = readUInt16(2);

    if (magic != EEPROM_MAGIC || isHeaderInvalid(totalUsed)) {
        factoryReset();  // reset if invalid or uninitialized
    }
}

size_t StorageManager::capacity() {
    return eepromSize;
}

///////////////////////////////////////////////////////////////
// Header & Factory Reset
///////////////////////////////////////////////////////////////

void StorageManager::factoryReset() {
    eraseEEPROM();

    // Write header: [magic][totalUsedBytes]
    writeUInt16(0, EEPROM_MAGIC); // write magic number at offset 0
    writeUInt16(2, HEADER_SIZE); // totalUsed at offset 2
    EEPROM.commit();
}

void StorageManager::writeTotalUsedBytes(uint16_t value) {
    writeUInt16(2, value);
}

uint16_t StorageManager::readTotalUsedBytes() {
    uint16_t magic = readUInt16(0); // magic number at offset 0
    uint16_t totalUsed = readUInt16(2);  // totalUsed at offset 2
    if (magic != EEPROM_MAGIC || isHeaderInvalid(totalUsed)) 
        return HEADER_SIZE;
    return totalUsed;
}

bool StorageManager::isHeaderInvalid(uint16_t headerValue) {
    // return (headerValue == 0xFFFF || headerValue == 0x0000 || headerValue > eepromSize);
    return (headerValue < HEADER_SIZE || headerValue > eepromSize);
}

void StorageManager::eraseEEPROM() {
    for (uint16_t i = 0; i < EEPROM.length(); i++) {
        EEPROM.write(i, 0xFF);
    }
    EEPROM.commit();
}

///////////////////////////////////////////////////////////////
// Key Existence Check
///////////////////////////////////////////////////////////////

bool StorageManager::keyExists(uint32_t key) {
    uint16_t totalUsed = readTotalUsedBytes();
    uint16_t address = HEADER_SIZE; // skip header

    while (address + ENTRY_OVERHEAD <= totalUsed) {
        uint16_t valueLength = readUInt16(address);
        address += sizeof(uint16_t);

        uint32_t storedKey = readUInt32(address);
        address += sizeof(uint32_t);

        if (storedKey == key) {
            return true; // key found
        }
        address += valueLength; // skip value
    }
    return false; // key not found
}

///////////////////////////////////////////////////////////////
// Write Operations
///////////////////////////////////////////////////////////////

bool StorageManager::writeKeyValue(uint32_t key, uint8_t* value, uint16_t valueLength) {
    if (!value || valueLength == 0)
        return false;

    if (keyExists(key)) {
        return false; // key already populated
    }

    if (valueLength + ENTRY_OVERHEAD > eepromSize)
        return false;  // Not enough EEPROM space

    uint16_t totalUsed = readTotalUsedBytes();
    uint16_t nextAddress = totalUsed;
    uint16_t newUsedBytes = nextAddress + ENTRY_OVERHEAD + valueLength;

    if (newUsedBytes > EEPROM.length())
        return false;  // EEPROM full

    // Write [length][key][data]
    writeUInt16(nextAddress, valueLength);
    nextAddress += sizeof(uint16_t);

    writeUInt32(nextAddress, key);
    nextAddress += sizeof(uint32_t);

    writeBytes(nextAddress, value, valueLength);

    // Update header
    writeTotalUsedBytes(newUsedBytes);

    EEPROM.commit();

    return true;
}

///////////////////////////////////////////////////////////////
// Read Operations
///////////////////////////////////////////////////////////////

bool StorageManager::readValueByKey(uint32_t key, uint8_t* dst, uint16_t expectedLen) {
    if (!dst || expectedLen == 0)
        return false;

    uint16_t totalUsed = readTotalUsedBytes();
    uint16_t address = HEADER_SIZE; // skip header

    while (address + ENTRY_OVERHEAD <= totalUsed) {
        uint16_t valueLength = readUInt16(address);
        address += sizeof(uint16_t);

        uint32_t storedKey = readUInt32(address);
        address += sizeof(uint32_t);

        if ((uint32_t)address + valueLength > EEPROM.length())
            break;  // corrupted entry

        if (storedKey == key) {
            readBytes(address, dst, std::min(valueLength, expectedLen));
            return true;
        }
        address += valueLength;
    }
    return false; // key not found
}

///////////////////////////////////////////////////////////////
// Debug & Inspection
///////////////////////////////////////////////////////////////

bool StorageManager::debugDumpKeys(uint8_t *dst, size_t maxLen, size_t &outLen) {
    uint16_t totalUsed = readTotalUsedBytes();
    if (totalUsed <= sizeof(uint16_t)) {
        outLen = 0;
        return false;
    }

    uint16_t address = HEADER_SIZE; // skip both magic and totalUsed
    size_t written = 0;

    while (address < totalUsed) {
        uint16_t entryLength = readUInt16(address);
        if (entryLength == 0 || entryLength > (totalUsed - address))
            break;

        uint32_t key = readUInt32(address + sizeof(uint16_t));
        if (written + sizeof(uint32_t) <= maxLen) {
            memcpy(dst + written, &key, sizeof(uint32_t));
            written += sizeof(uint32_t);
        }

        address += ENTRY_OVERHEAD + entryLength;
    }

    outLen = written;
    return (written > 0);
}

///////////////////////////////////////////////////////////////
// EEPROM Read/Write Utility Helpers
///////////////////////////////////////////////////////////////

void StorageManager::writeUInt16(uint16_t address, uint16_t value) {
    EEPROM.write(address, (value >> 8) & 0xFF); // high byte first
    EEPROM.write(address + 1, value & 0xFF);    // low byte second
}

uint16_t StorageManager::readUInt16(uint16_t address) {
    return (EEPROM.read(address) << 8) | EEPROM.read(address + 1);
}

void StorageManager::writeUInt32(uint16_t address, uint32_t value) {
    for (int i = 0; i < 4; i++)
        EEPROM.write(address + i, (value >> (8 * i)) & 0xFF);
}

uint32_t StorageManager::readUInt32(uint16_t address) {
    uint32_t value = 0;
    for (int i = 0; i < 4; i++)
        value |= (uint32_t)EEPROM.read(address + i) << (8 * i);
    return value;
}

void StorageManager::writeBytes(uint16_t address, const uint8_t *data, uint16_t len) {
    for (uint16_t i = 0; i < len; i++)
        EEPROM.write(address + i, data[i]);
}

void StorageManager::readBytes(uint16_t address, uint8_t *dst, uint16_t len) {
    for (uint16_t i = 0; i < len; i++)
        dst[i] = EEPROM.read(address + i);
}
