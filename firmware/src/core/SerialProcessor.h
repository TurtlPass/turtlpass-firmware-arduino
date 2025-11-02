#ifndef SERIAL_PROCESSOR_H
#define SERIAL_PROCESSOR_H

#include <cstddef>
#include <cstdint>
#include "core/CommandProcessor.h"


/**
 * @brief Processes protobuf frames received via Serial.
 * 
 * Handles:
 * - Reading bytes from Serial
 * - Assembling frames with 2-byte length prefix
 * - Handling timeouts for incomplete frames
 * - Delegating complete frames to CommandProcessor
 */
class SerialProcessor {
public:
    /**
     * @brief Constructs a SerialProcessor instance.
     * 
     * @param cmdProcessor Reference to the CommandProcessor to handle parsed commands.
     */
    SerialProcessor(CommandProcessor &cmdProcessor);

    /**
     * @brief Must be called in Arduino loop().
     * 
     * Reads incoming bytes, assembles frames, checks for timeouts,
     * and calls CommandProcessor when a full frame is available.
     */
    void loop();

private:
    CommandProcessor &commandProcessor_; /**< Reference to command processor */

    uint8_t buffer_[512];     /**< Temporary buffer for assembling a frame */
    size_t bytesRead_;        /**< Number of bytes currently read into buffer */
    size_t expectedLength_;   /**< Length of the current frame payload */
    unsigned long lastByteTime_; /**< Timestamp of the last byte received */

    static const unsigned long SERIAL_TIMEOUT_MS = 500; /**< Timeout for incomplete frames */
};

#endif
