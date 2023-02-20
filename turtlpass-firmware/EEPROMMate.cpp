/*
  EEPROMMate Library

  Description: A library for storing key-value pairs in the EEPROM with AES-256 encryption support.
*/
#include "EEPROMMate.h"

EEPROMMate::EEPROMMate() {}

void EEPROMMate::begin(size_t eepromSize) {
  EEPROM.begin(eepromSize);
}

void EEPROMMate::commit() {
  // writes the updated data to flash, so next reboot it will be readable
  EEPROM.commit();
  Serial.println("EEPROM.commit()");
}

void EEPROMMate::factoryReset() {
  // write 0 in all bytes of the EEPROM
  for (uint16_t i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0);
  }
}

///////////
// Write //
///////////

void EEPROMMate::writeTotalUsedBytes(uint16_t value) {
  writeIntToEEPROM(0, value);
}

bool EEPROMMate::writeDataEncrypted(uint8_t* key, uint8_t keyLength, uint8_t* value, uint16_t valueLength, uint8_t* aesKey, uint8_t* aesIv) {
  int dataLength = valueLength;
  if ((valueLength % 16) != 0) { // keep the length multiple of 16
    dataLength += 16 - (valueLength % 16);
  }
  uint8_t encryptedValue[dataLength];
  bool encryptResult = aes256.encrypt(aesKey, aesIv, dataLength, (unsigned char*)value, encryptedValue);
  if (!encryptResult) {
    Serial.println("Encryption failed");
    return false; // Encryption failed
  }
  return writeData(key, keyLength, encryptedValue, valueLength);
}

bool EEPROMMate::writeData(uint8_t* key, uint8_t keyLength, uint8_t* value, uint16_t valueLength) {
  uint32_t hash = fastCrc.crc32(key, keyLength);
  return writeKeyValue(hash, value, valueLength);
}

bool EEPROMMate::writeKeyValue(uint32_t key, uint8_t* value, uint16_t valueLength) {
  uint8_t* tmp = new uint8_t[valueLength];
  bool keyExists = readValueByKey(key, tmp, valueLength);
  delete [] tmp;
  if (keyExists) {
    Serial.println("Key already exists in the EEPROM");
    return false; // key already exists in the EEPROM
  }
  if (valueLength == 0) {
    Serial.println("Value length cannot be zero");
    return false; // value length cannot be zero
  }
  uint16_t totalUsedBytes = readTotalUsedBytes();
  if ((totalUsedBytes + sizeof(uint16_t) + sizeof(uint32_t) + valueLength) > EEPROM.length()) {
    Serial.print("Value >>> "); Serial.println((totalUsedBytes + sizeof(uint16_t) + sizeof(uint32_t) + valueLength));
    Serial.print("EEPROM.length() >>> "); Serial.println(EEPROM.length());
    Serial.println("EEPROM does not have enough free space");
    return false; // EEPROM does not have enough free space
  }
  uint16_t addressEntry;
  if (totalUsedBytes == 0) { // empty
    addressEntry = sizeof(uint16_t); // skip header
  } else {
    addressEntry = totalUsedBytes + 1;
  }
  // Writing Length
  uint16_t entryLength = valueLength;
  writeIntToEEPROM(addressEntry, entryLength);
  // Writing Key
  writeLongToEEPROM(addressEntry + sizeof(uint16_t), key);
  // Writing Value
  for (uint16_t i = 0; i < valueLength; i++) {
    EEPROM.write(addressEntry + sizeof(uint16_t) + sizeof(uint32_t) + i, value[i]);
  }
  totalUsedBytes = addressEntry + sizeof(uint16_t) + sizeof(uint32_t) + valueLength;
  writeTotalUsedBytes(totalUsedBytes);
  return true;
}

//////////
// Read //
//////////

uint16_t EEPROMMate::readTotalUsedBytes() {
  return readIntFromEEPROM(0);
}

bool EEPROMMate::readValueDecrypted(uint8_t* key, uint8_t keyLength, uint8_t* dstValue, uint16_t dstValueLength, uint8_t* aesKey, uint8_t* aesIv) {
  uint8_t encryptedValue[dstValueLength];
  // Read the encrypted value
  bool result = readValue(key, keyLength, encryptedValue, dstValueLength);
  if (!result) {
    return false; // failed to read value
  }
  // uint8_t decryptedValue[dstValueLength];
  uint8_t* decryptedValue = new uint8_t[dstValueLength];
  bool decryptResult = aes256.decrypt(aesKey, aesIv, dstValueLength, encryptedValue, decryptedValue);
  if (!decryptResult) {
    return false; // decryption failed
  }
  // copy the decrypted value to the destination
  memcpy(dstValue, decryptedValue, dstValueLength);
  delete [] decryptedValue;
  return result;
}

bool EEPROMMate::readValue(uint8_t* keyInput, uint8_t keyLength, uint8_t* dstValue, uint16_t dstValueLength) {
  uint32_t key = fastCrc.crc32(keyInput, keyLength);
  return readValueByKey(key, dstValue, dstValueLength);
}

uint16_t EEPROMMate::readValueLength(uint8_t* keyInput, uint8_t keyLength) {
  uint32_t key = fastCrc.crc32(keyInput, keyLength);
  uint16_t totalUsedBytes = readTotalUsedBytes();
  if (totalUsedBytes == 0) {
    return -1; // there is nothing to read
  }
  uint32_t currentKey;
  uint16_t address = INDEX_FIRST_ENTRY;

  while (address < totalUsedBytes) {
    uint16_t entryLength = readIntFromEEPROM(address);
    if (entryLength == 0 || entryLength > totalUsedBytes) {
      return -1; // invalid entry length
    }
    currentKey = readLongFromEEPROM(address + sizeof(uint16_t));
    if (currentKey == key) {
      return entryLength;
    }
    address = address + sizeof(uint16_t) + sizeof(uint32_t) + entryLength + 1;
  }
  return -1; // key not found
}

bool EEPROMMate::readValueByKey(uint32_t key, uint8_t *dstValue, uint16_t dstValueLength) {
  uint16_t totalUsedBytes = readTotalUsedBytes();
  if (totalUsedBytes == 0) {
    return false; // there is nothing to read
  }
  if (dstValue == NULL) {
    return false; // destination buffer is NULL
  }
  uint32_t currentKey;
  uint16_t address = INDEX_FIRST_ENTRY;

  while (address < totalUsedBytes) {
    uint16_t entryLength = readIntFromEEPROM(address);
    if (entryLength == 0 || entryLength > totalUsedBytes) {
      return false; // invalid entry length
    }
    currentKey = readLongFromEEPROM(address + sizeof(uint16_t));
    if (currentKey == key) {
      uint8_t valueLenght = entryLength;
      uint8_t value[valueLenght];
      for (uint16_t i = 0; i < valueLenght; i++) {
        value[i] = EEPROM.read(address + sizeof(uint16_t) + sizeof(uint32_t) + i);
      }
      // check if the destination buffer is big enough to hold the data
      if (sizeof(value) > dstValueLength) {
        return false; // destination buffer is too small
      }
      // copy the encrypted value to the destination
      memcpy(dstValue, value, sizeof(value));
      return true;
    }
    address = address + sizeof(uint16_t) + sizeof(uint32_t) + entryLength + 1;
  }
  return false; // key not found
}

bool EEPROMMate::readAllSavedData() {
  uint16_t totalUsedBytes = readTotalUsedBytes();
  Serial.print("Total Used Bytes: ");
  Serial.print(totalUsedBytes);
  Serial.print("/");
  Serial.println(EEPROM.length());
  if (totalUsedBytes == 0) {
    Serial.println("There is nothing to read");
    return false;
  }
  uint16_t addressEntry = INDEX_FIRST_ENTRY;
  uint32_t currentKey;
  char value[4096];
  uint16_t address = INDEX_FIRST_ENTRY;
  uint8_t index = 0;

  while (address < totalUsedBytes) {
    uint16_t entryLength = readIntFromEEPROM(address);
    if (entryLength == 0 || entryLength > totalUsedBytes) {
      Serial.println("Error: invalid entry length");
      return false;
    }
    Serial.print(index);
    Serial.print(":");
    currentKey = readLongFromEEPROM(address + sizeof(uint16_t));
    Serial.print(currentKey, HEX);
    Serial.print(":");
    uint8_t valueLenght = entryLength;
    uint8_t value[valueLenght];
    for (uint16_t i = 0; i < valueLenght; i++) {
      value[i] = EEPROM.read(address + sizeof(uint16_t) + sizeof(uint32_t) + i);
    }
    printArrayHex(value, sizeof(value) / sizeof(value[0]));
    address = address + sizeof(uint16_t) + sizeof(uint32_t) + entryLength + 1;
    index++;
  }
  return true;
}

/////////////
// PRIVATE //
/////////////

uint16_t EEPROMMate::readIntFromEEPROM(uint16_t address) {
  byte _1 = EEPROM.read(address);
  byte _2 = EEPROM.read(address + 1);
  return (_1 << 8) + _2;
}

uint32_t EEPROMMate::readLongFromEEPROM(uint16_t address) {
  if (address + 3 >= EEPROM.length()) {
    Serial.println("Error: Address out of range");
    return 0;
  }
  // read the 4 bytes from the simulated EEPROM memory
  uint32_t byte1 = EEPROM.read(address);
  uint32_t byte2 = EEPROM.read(address + 1);
  uint32_t byte3 = EEPROM.read(address + 2);
  uint32_t byte4 = EEPROM.read(address + 3);
  // return the recomposed long by using bit shifting and bitwise OR
  return (byte1) | (byte2 << 8) | (byte3 << 16) | (byte4 << 24);
}

void EEPROMMate::writeIntToEEPROM(uint16_t address, uint16_t value) {
  byte _1 = value >> 8;
  byte _2 = value & 0xFF;
  EEPROM.write(address, _1);
  EEPROM.write(address + 1, _2);
}

void EEPROMMate::writeLongToEEPROM(uint16_t address, uint32_t value) {
  if (address + 3 >= EEPROM.length()) {
    Serial.println("Error: Address out of range");
    return;
  }
  // decomposition from a long to 4 bytes by using bitshift
  // 1 = most significant -> 4 = least significant byte
  byte byte1 = value & 0xFF;
  byte byte2 = (value >> 8) & 0xFF;
  byte byte3 = (value >> 16) & 0xFF;
  byte byte4 = (value >> 24) & 0xFF;
  // write the 4 bytes into the simulated EEPROM memory
  EEPROM.write(address, byte1);
  EEPROM.write(address + 1, byte2);
  EEPROM.write(address + 2, byte3);
  EEPROM.write(address + 3, byte4);
}

void EEPROMMate::printArrayHex(uint8_t arr[], uint16_t arrayLength) {
  for (int i = 0; i < arrayLength; i++) {
    Serial.print(arr[i], HEX);
  }
  Serial.println();
}