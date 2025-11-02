#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#include "pb.h"
#include "proto/turtlpass.pb.h"
#include "storage/SeedManager.h"
#include "crypto/Kdf.h"
#include "ui/LedManager.h"
#include "InternalState.h"
#include <cstddef>
#include "system/SystemInfo.h"

#define DEFAULT_PASS_SIZE 100  // 100 characters by default
#define MAX_PASS_SIZE 128
#define MAX_ENTROPY_SIZE 64

/**
* @class CommandProcessor
* @brief Handles incoming protobuf commands and delegates logic for password generation, seed management, and device info queries.
*/
class CommandProcessor {
public:
    /**
    * @brief Construct a new CommandProcessor.
    * @param ledManager Reference to LED manager for state indication.
    * @param seedManager Reference to SeedManager for seed operations.
    * @param kdf Reference to key derivation function implementation.
    * @param outputBuffer Pointer to output password buffer.
    * @param outputBufferSize Size of the output password buffer.
    */
    CommandProcessor(SeedManager& seedManager, Kdf& kdf, LedManager& ledManager, InternalState& state, uint8_t* outputBuffer, size_t outputBufferSize);

    /**
     * @brief Processes a decoded protobuf command buffer.
     * @param data Pointer to command data bytes.
     * @param length Length of the protobuf message.
     */
    void processProtoCommand(const uint8_t* data, size_t length);

    /**
     * @brief Returns the currently selected seed slot based on the active LED color index.
     * 
     * This function maps the LED color index (managed by LedManager) to a seed slot number.
     * The mapping is 1-based — i.e., LED color index `0` corresponds to seed slot `1`.
     * 
     * @return uint8_t The selected seed slot number (1-based index).
     */
    uint8_t getSelectedSeedSlot();

    /**
     * @brief Retrieves the currently selected seed bytes.
     * @param outSeed Output buffer for the seed.
     * @param outSize Size of the output buffer.
     * @param seedSize Expected size of the seed.
     * @return true if successful, false otherwise.
     */
    bool getSelectedSeed(char *outSeed, size_t outSize, size_t seedSize = SeedManager::SEED_SIZE);

    /**
     * @brief Derives the default password using the selected seed.
     *        Used for long-touch operations.
     * @return true if the password was successfully derived and written to outputBuffer, false otherwise.
     */
    bool deriveDefaultPassword();

    /**
     * @brief Returns a pointer to the internal output buffer.
     *        Intended for read-only access or HID typing.
     */
    uint8_t* getOutputBuffer();

    /**
     * @brief Returns the size of the internal output buffer.
     */
    size_t getOutputBufferSize();

    /**
     * @brief Clears the internal output buffer.
     */
    void clearOutputBuffer();

private:
    SeedManager& seedManager_;
    Kdf& kdf_;
    LedManager& ledManager_;
    InternalState& state_;
    // char* outputBuffer_;
    uint8_t *outputBuffer_;
    size_t outputBufferSize_;

    /**
     * @brief Handles the GET_DEVICE_INFO command type.
     *        Builds and sends a protobuf response containing device information.
     */
    void handleGetDeviceInfo();

    /**
     * @brief Handles the GENERATE_PASSWORD command type.
     *        Uses the KDF and selected seed to derive a password based on provided parameters.
     * @param command Reference to decoded turtlpass_Command protobuf object.
     */
    void handleGeneratePassword(const turtlpass_Command &command);

    /**
     * @brief Handles the INITIALIZE_SEED command type.
     *        Stores and verifies a new seed in the SeedManager.
     * @param command Reference to decoded turtlpass_Command protobuf object.
     */
    void handleInitializeSeed(const turtlpass_Command &command);

    /**
     * @brief Handles the FACTORY_RESET command type.
     *        Resets all stored seeds to factory default.
     */
    void handleFactoryReset();
};

#endif // COMMAND_PROCESSOR_H
