#include "CommandProcessor.h"
#include "proto/ProtoHelper.h"
#include "CommandProcessor.h"
#include "proto/ProtoHelper.h"
#include <cstring>

CommandProcessor::CommandProcessor(SeedManager& seedManager, Kdf& kdf, LedManager& ledManager, InternalState& state, uint8_t* outputBuffer, size_t outputBufferSize)
: seedManager_(seedManager), kdf_(kdf), ledManager_(ledManager), state_(state), outputBuffer_(outputBuffer), outputBufferSize_(outputBufferSize) {
    // ensure output buffer is zeroed
    if (outputBuffer_ && outputBufferSize_ > 0) {
        memset(outputBuffer_, 0, outputBufferSize_);
    }
}

void CommandProcessor::processProtoCommand(const uint8_t* data, size_t length) {
    turtlpass_Command command = turtlpass_Command_init_zero;
    pb_istream_t stream = pb_istream_from_buffer(data, length);

    if (!pb_decode(&stream, turtlpass_Command_fields, &command)) {
        sendErrorResponse(turtlpass_ErrorCode_PROTO_DECODING_FAILED);
        return;
    }
    switch (command.type) {
        case turtlpass_CommandType_GET_DEVICE_INFO:
            handleGetDeviceInfo();
            break;

        case turtlpass_CommandType_GENERATE_PASSWORD:
            handleGeneratePassword(command);
            break;

        case turtlpass_CommandType_INITIALIZE_SEED:
            handleInitializeSeed(command);
            break;

        case turtlpass_CommandType_FACTORY_RESET:
            handleFactoryReset();
            break;

        default:
            sendErrorResponse(turtlpass_ErrorCode_INVALID_COMMAND);
            state_ = IDLE;
            break;
    }
}

bool CommandProcessor::getSelectedSeed(char* outSeed, size_t outSize, const size_t seedSize) {
    if (!outSeed || outSize < seedSize + 1) {
        return false; // buffer too small or null pointer
    }
    uint8_t seed[seedSize];
    uint8_t seedSlot = getSelectedSeedSlot();
    if (!seedManager_.getSeed(seedSlot, seed, seedSize)) {
        return false;
    }
    for (size_t i = 0; i < seedSize; ++i) {
        outSeed[i] = static_cast<char>(seed[i]);
    }
    outSeed[seedSize] = '\0';
    return true;
}

uint8_t CommandProcessor::getSelectedSeedSlot() {
  return ledManager_.getColorIndex() + 1;
}

bool CommandProcessor::deriveDefaultPassword() {
    char seed[SeedManager::SEED_SIZE + 1];
    if (!getSelectedSeed(seed, sizeof(seed))) {
        return false;
    }
    bool result = kdf_.derivatePass(outputBuffer_, DEFAULT_PASS_SIZE, const_cast<char*>("default"), seed);
    return result && outputBuffer_[0] != 0;
}

uint8_t* CommandProcessor::getOutputBuffer() {
    return outputBuffer_;
}

size_t CommandProcessor::getOutputBufferSize() {
    return outputBufferSize_;
}

void CommandProcessor::clearOutputBuffer() {
    memset(outputBuffer_, 0, outputBufferSize_);
}

// ---------------- Command Handlers ----------------

void CommandProcessor::handleGetDeviceInfo() {
    turtlpass_Response response = turtlpass_Response_init_zero;
    deviceInfo(response);
    sendProtoResponse(response);
    state_ = IDLE;
}

void CommandProcessor::handleGeneratePassword(const turtlpass_Command& command) {
    if (command.which_parameters != turtlpass_Command_gen_pass_tag) {
        sendErrorResponse(turtlpass_ErrorCode_INVALID_PARAMS);
        state_ = IDLE;
        return;
    }

    const auto &params = command.parameters.gen_pass;
    if (params.entropy.size == 0 || params.entropy.size > MAX_ENTROPY_SIZE) {
        sendErrorResponse(turtlpass_ErrorCode_INVALID_ENTROPY_LENGTH);
        state_ = IDLE;
        return;
    }
    if (params.length < 1 || params.length > MAX_PASS_SIZE) {
        sendErrorResponse(turtlpass_ErrorCode_INVALID_PASSWORD_LENGTH);
        state_ = IDLE;
        return;
    }

    uint32_t pass_len = params.length;
    if (pass_len > outputBufferSize_ - 1) pass_len = outputBufferSize_ - 1;

    // get seed
    char seed[SeedManager::SEED_SIZE + 1];
    if (!getSelectedSeed(seed, sizeof(seed))) {
        sendErrorResponse(turtlpass_ErrorCode_SEED_NOT_INITIALIZED);
        state_ = IDLE;
        return;
    }

    const char* entropy = reinterpret_cast<const char*>(params.entropy.bytes);

    bool result = false;
    switch (params.charset) {
        case turtlpass_Charset_NUMBERS_ONLY:
            result = kdf_.derivatePassNumbersOnly(outputBuffer_, pass_len, entropy, seed);
            break;
        case turtlpass_Charset_LETTERS_ONLY:
            result = kdf_.derivatePassLettersOnly(outputBuffer_, pass_len, entropy, seed);
            break;
        case turtlpass_Charset_LETTERS_NUMBERS_SYMBOLS:
            result = kdf_.derivatePassWithSymbols(outputBuffer_, pass_len, entropy, seed);
            break;
        default:
            result = kdf_.derivatePass(outputBuffer_, pass_len, entropy, seed);
            break;
    }

    if (result) {
        ledManager_.setPulsing();
        sendSuccessResponse();
        state_ = PASSWORD_READY;
    } else {
        if (outputBuffer_) memset(outputBuffer_, 0, outputBufferSize_);
        sendErrorResponse(turtlpass_ErrorCode_PASSWORD_FAILED);
        state_ = IDLE;
    }
}

void CommandProcessor::handleInitializeSeed(const turtlpass_Command& command) {
    if (command.which_parameters != turtlpass_Command_init_seed_tag) {
        sendErrorResponse(turtlpass_ErrorCode_INVALID_PARAMS);
        state_ = IDLE;
        return;
    }
    const turtlpass_InitializeSeedParams &params = command.parameters.init_seed;
    if (params.seed.size != SeedManager::SEED_SIZE) {
        sendErrorResponse(turtlpass_ErrorCode_INVALID_SEED_LENGTH);
        state_ = IDLE;
        return;
    }
    uint8_t seedSlot = getSelectedSeedSlot();
    auto result = seedManager_.initializeSeed(
        seedSlot,
        params.seed.bytes,
        params.seed.size
    );
    if (result == SeedManager::SeedInitResult::OK) {
        sendSuccessResponse();
        state_ = IDLE;
        return;
    }
    const char* msg = nullptr;
    switch (result) {
        case SeedManager::SeedInitResult::INVALID_INPUT:
            msg = "Invalid seed input";
            break;
        case SeedManager::SeedInitResult::INVALID_SLOT:
            msg = "Invalid seed slot";
            break;
        case SeedManager::SeedInitResult::ALREADY_POPULATED:
            msg = "Seed slot already populated";
            break;
        case SeedManager::SeedInitResult::WRITE_FAIL:
            msg = "Failed to write encrypted seed to EEPROM";
            break;
        case SeedManager::SeedInitResult::READ_FAIL:
            msg = "Failed to read encrypted seed back from EEPROM";
            break;
        case SeedManager::SeedInitResult::VERIFY_FAIL:
            msg = "Seed verification failed after write";
            break;
        default:
            msg = "Unknown seed initialization result";
            break;
    }
    sendErrorMessageResponse(turtlpass_ErrorCode_INTERNAL_ERROR, msg);
    state_ = IDLE;
}

void CommandProcessor::handleFactoryReset() {
    seedManager_.factoryReset();
    sendSuccessResponse();
    state_ = IDLE;
}
