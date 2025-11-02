#include "SerialProcessor.h"
#include <cstring>
#include "proto/ProtoHelper.h"

SerialProcessor::SerialProcessor(CommandProcessor &cmdProcessor)
    : commandProcessor_(cmdProcessor), bytesRead_(0), expectedLength_(0), lastByteTime_(0) {}

// Protobuf Serial Reader
void SerialProcessor::loop() {
    while (Serial.available() > 0) {
        uint8_t byte = Serial.read();
        lastByteTime_ = millis();

        if (bytesRead_ < 2) {
            // Read 2-byte length prefix
            buffer_[bytesRead_++] = byte;

            if (bytesRead_ == 2) {
                expectedLength_ = buffer_[0] | (buffer_[1] << 8);
                
                // Sanity check for frame size
                if (expectedLength_ == 0 || expectedLength_ > sizeof(buffer_) - 2) {
                    sendErrorMessageResponse(turtlpass_ErrorCode_INTERNAL_ERROR, "<PROTO-BAD-LENGTH>");
                    
                    // Shift buffer left by one and try again with the next byte
                    buffer_[0] = buffer_[1];
                    bytesRead_ = 1;
                    expectedLength_ = 0;
                    continue;
                }
            }
        } else {
            // Read message payload
            buffer_[bytesRead_++] = byte;

            // Full message received
            if (bytesRead_ == expectedLength_ + 2) {
                commandProcessor_.processProtoCommand(buffer_ + 2, expectedLength_);
                bytesRead_ = 0;
                expectedLength_ = 0;
                memset(buffer_, 0, sizeof(buffer_));
                return;
            }
        }
    }

    // Timeout handling: reset partial frames if input stalls
    if (bytesRead_ > 0 && millis() - lastByteTime_ > SERIAL_TIMEOUT_MS) {
        bytesRead_ = 0;
        expectedLength_ = 0;
        memset(buffer_, 0, sizeof(buffer_));
        sendErrorMessageResponse(turtlpass_ErrorCode_INTERNAL_ERROR, "<PROTO-TIMEOUT>");
    }
}
