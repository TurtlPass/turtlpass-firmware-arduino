#ifndef PROTO_HELPER_H
#define PROTO_HELPER_H

#include <SoftwareSerial.h>
#include "pb.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "proto/turtlpass.pb.h"


void sendSuccessResponse();
void sendSuccessBytesResponse(uint8_t* data, const uint16_t length);
void sendErrorResponse(const turtlpass_ErrorCode error);
void sendErrorMessageResponse(const turtlpass_ErrorCode error, const char* msg);
void sendProtoResponse(const turtlpass_Response &response);

#endif // PROTO_HELPER_H
