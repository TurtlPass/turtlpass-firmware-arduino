#include "system/SystemInfo.h"

void deviceInfo(turtlpass_Response &response) {
    // Mark this response as a success
    response.success = true;
    response.error = turtlpass_ErrorCode_NONE;
    response.has_device_info = true;  // Required for nanopb "has_" fields

    turtlpass_DeviceInfo &info = response.device_info;

    // Populate version strings (Nanopb auto-truncates if longer than max_size)
    snprintf(response.device_info.turtlpass_version,
             sizeof(response.device_info.turtlpass_version),
             "%s", TURTLPASS_VERSION);

    snprintf(response.device_info.arduino_version,
             sizeof(response.device_info.arduino_version),
             "%d", ARDUINO);

    snprintf(response.device_info.compiler_version,
             sizeof(response.device_info.compiler_version),
             "%s", __VERSION__);

    snprintf(response.device_info.nanopb_version,
             sizeof(response.device_info.nanopb_version),
             "%s", NANOPB_VERSION);

    snprintf(response.device_info.board_name,
             sizeof(response.device_info.board_name),
             "%s", PIO_BOARD_NAME);

    // Populate unique board ID
    pico_unique_board_id_t unique_id;
    pico_get_unique_board_id(&unique_id);

    memcpy(response.device_info.unique_board_id.bytes,
           unique_id.id,
           PICO_UNIQUE_BOARD_ID_SIZE_BYTES);

    response.device_info.unique_board_id.size = PICO_UNIQUE_BOARD_ID_SIZE_BYTES;
}
