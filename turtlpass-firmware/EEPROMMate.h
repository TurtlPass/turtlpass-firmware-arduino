/*
  EEPROMMate Library

  Description: A library for storing key-value pairs in the EEPROM.
*/
// ------------------------------------------------------------------------------------------------------ ...
// | HEADER |                                      KEY/VALUE LIST                                         ...
// ------------------------------------------------------------------------------------------------------ ...
// | Used   |           1# Entry          |           2# Entry           |           3# Entry           | ...
// | Bytes  | V.Length |   Key   | Value  | V.Length |   Key   |  Value  | V.Length |   Key   |  Value  | ...
// ------------------------------------------------------------------------------------------------------ ...
// | 2Bytes | 2 Bytes  | 4 Bytes | *Bytes | 2 Bytes  | 4 Bytes | * Bytes | 2 Bytes  | 4 Bytes | * Bytes | ...
// ------------------------------------------------------------------------------------------------------ ...
// EEPROM documentation: https://arduino-pico.readthedocs.io/en/latest/eeprom.html
// CRC32 online tool:    https://crc32.online
//
#ifndef EEPROM_MATE_H
#define EEPROM_MATE_H

#include <EEPROM.h>  // include the EEPROM library for Raspberry Pi Pico RP2040
#include <FastCRC.h> // https://github.com/FrankBoesing/FastCRC
#include <stdlib.h>
#include <stdint.h>
#include <cstring>

#if defined(__TURTLPASS_EEPROM_SIZE__)
#define SIZE_EEPROM __TURTLPASS_EEPROM_SIZE__
#else
#define SIZE_EEPROM 4096
#endif

class EEPROMMate {
  public:
    EEPROMMate();
    void begin(size_t eepromSize);
    void commit();
    void factoryReset();
    bool writeData(uint8_t* key, uint8_t keyLength, uint8_t* value, uint16_t valueLength);
    bool writeKeyValue(uint32_t key, uint8_t* value, uint16_t valueLength);
    bool readValue(uint8_t* key, uint8_t keyLength, uint8_t* dstValue, uint16_t dstValueLength);
    uint16_t readValueLength(uint8_t* keyInput, uint8_t keyLength);
    bool readValueByKey(uint32_t key, uint8_t *dstValue, uint16_t dstValueLength);
    bool readAllSavedData();

  private:
    FastCRC32 fastCrc;
    void writeTotalUsedBytes(uint16_t value);
    void writeIntToEEPROM(uint16_t address, uint16_t value);
    void writeLongToEEPROM(uint16_t address, uint32_t value);
    uint16_t readTotalUsedBytes();
    uint16_t readIntFromEEPROM(uint16_t address);
    uint32_t readLongFromEEPROM(uint16_t address);
    void printArrayHex(uint8_t arr[], uint16_t arrayLength);
    const int INDEX_FIRST_ENTRY = 2;
};

#endif // EEPROM_MATE_H
