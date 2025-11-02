#include "HidKeyboard.h"

// Build a constant conversion table from ASCII → (shift, keycode)
static const uint8_t conv_table[128][2] = { HID_ASCII_TO_KEYCODE };

static const uint8_t HID_REPORT_DESCRIPTOR_KEYBOARD[] = {
  TUD_HID_REPORT_DESC_KEYBOARD()
};

// Create HID report descriptor (boot keyboard)
Adafruit_USBD_HID usb_hid(
    HID_REPORT_DESCRIPTOR_KEYBOARD,
    sizeof(HID_REPORT_DESCRIPTOR_KEYBOARD),
    HID_ITF_PROTOCOL_KEYBOARD,
    2, // interval (ms)
    true
);

// Initialize HID
void hidKeyboardInit() {
  usb_hid.begin();
  // Wait for USB to be ready
  while (!TinyUSBDevice.mounted()) {
    delay(10);
  }
}

// Send a single ASCII character via USB HID
void hidSendKey(char c) {
  if (c < 0 || c >= 128) return; // ignore non-printable chars

  uint8_t shift = conv_table[(uint8_t)c][0];
  uint8_t keycode = conv_table[(uint8_t)c][1];

  if (keycode == 0) return; // unmapped char

  uint8_t modifier = shift ? KEYBOARD_MODIFIER_LEFTSHIFT : 0;
  uint8_t keycodes[6] = { keycode, 0, 0, 0, 0, 0 };

  // Send the keypress
  tud_hid_keyboard_report(0, modifier, keycodes);
  sleep_ms(8);

  // Release all keys
  tud_hid_keyboard_report(0, 0, NULL);
  sleep_ms(5);
}

// Type a string
void hidTypeString(const char* str) {
  while (*str) {
    hidSendKey(*str++);
  }
}

// Send Enter
void hidSendEnter() {
  hidSendKey('\n');
}
