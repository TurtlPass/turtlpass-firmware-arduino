#include "proto/ProtoHelper.h"


void sendSuccessResponse() {
    turtlpass_Response response = turtlpass_Response_init_zero;
    response.success = true;
    response.error = turtlpass_ErrorCode_NONE;
    sendProtoResponse(response);
}

void sendSuccessBytesResponse(uint8_t* data, const uint16_t length) {
    turtlpass_Response response = turtlpass_Response_init_zero;
    response.success = true;
    response.error = turtlpass_ErrorCode_NONE;
    response.data.size = length;
    memcpy(response.data.bytes, data, length);
    sendProtoResponse(response);
}

void sendErrorResponse(const turtlpass_ErrorCode error) {
    turtlpass_Response response = turtlpass_Response_init_zero;
    response.success = false;
    response.error = error;
    sendProtoResponse(response);
}

void sendErrorMessageResponse(const turtlpass_ErrorCode error, const char* msg) {
    turtlpass_Response response = turtlpass_Response_init_zero;
    response.success = false;
    response.error = error;

    size_t msg_len = strlen(msg);
    if (msg_len > sizeof(response.data.bytes)) {
        msg_len = sizeof(response.data.bytes);
    }
    memcpy(response.data.bytes, msg, msg_len);
    response.data.size = msg_len;

    if (response.data.size > sizeof(response.data.bytes)) {
        response.data.size = sizeof(response.data.bytes);
        // add truncation marker
        const char *trunc = "...";
        size_t trunc_len = strlen(trunc);
        if (response.data.size > trunc_len) {
            memcpy(response.data.bytes + response.data.size - trunc_len, trunc, trunc_len);
        }
    }
    sendProtoResponse(response);
}

void sendProtoResponse(const turtlpass_Response &response) {
    uint8_t buffer[1024];  // Enough for 512 bytes data + overhead
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

    if (pb_encode(&stream, turtlpass_Response_fields, &response)) {
        // --- Success: send normally ---
        // Send 2-byte length prefix
        Serial.write((uint8_t)(stream.bytes_written & 0xFF));
        Serial.write((uint8_t)((stream.bytes_written >> 8) & 0xFF));
        // Send encoded protobuf bytes
        Serial.write(buffer, stream.bytes_written);
    } else {
        // --- Failure: report encoding error back as a structured Response ---
        turtlpass_Response error_response = turtlpass_Response_init_zero;
        error_response.success = false;
        error_response.error = turtlpass_ErrorCode_PROTO_ENCODING_FAILED;

        const char *err = PB_GET_ERROR(&stream);
        snprintf((char *)buffer, sizeof(buffer), "Encoding failed: %s", err ? err : "unknown");

        error_response.data.size = strlen((char *)buffer);
        memcpy(error_response.data.bytes, buffer, error_response.data.size);

        pb_ostream_t err_stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        if (pb_encode(&err_stream, turtlpass_Response_fields, &error_response)) {
            Serial.write((uint8_t)(err_stream.bytes_written & 0xFF));
            Serial.write((uint8_t)((err_stream.bytes_written >> 8) & 0xFF));
            Serial.write(buffer, err_stream.bytes_written);
        } else {
            // Worst case fallback — print to debug serial
            Serial.print("❌ sendProtoResponse: DOUBLE encoding failure: ");
            Serial.println(PB_GET_ERROR(&err_stream));
        }
    }
}
