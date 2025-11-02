#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include <Arduino.h>
#include "pico/unique_id.h"
#include "pb.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "proto/turtlpass.pb.h"

#if defined(__TURTLPASS_VERSION__)
#define TURTLPASS_VERSION __TURTLPASS_VERSION__
#else
#define TURTLPASS_VERSION "development"
#endif

void deviceInfo(turtlpass_Response &response);


#endif // SYSTEM_INFO_H
