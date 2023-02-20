#include "Info.h"

void deviceInfo() {
    Serial.println("TurtlPass Firmware Version: " + String(TURTLPASS_VERSION));
    Serial.println("Arduino Version: " + String(ARDUINO));
    Serial.println("Compiler Version: " + String(__VERSION__));
    pico_unique_board_id_t unique_id;
    pico_get_unique_board_id(&unique_id);
    Serial.print("Unique Board ID: ");
    for (int i = 0; i < PICO_UNIQUE_BOARD_ID_SIZE_BYTES; i++) {
      Serial.print(unique_id.id[i], HEX);
    }
    Serial.println();
}
