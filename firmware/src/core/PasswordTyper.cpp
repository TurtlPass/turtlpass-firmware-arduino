#include "PasswordTyper.h"
#include "keyboard/HidKeyboard.h"
#include <cstring>

PasswordTyper::PasswordTyper(LedManager &led, uint8_t *buffer, size_t size)
    : ledManager_(led), buffer_(buffer), bufferSize_(size) {}

void PasswordTyper::type() {
    if (memchr(buffer_, 0, bufferSize_) != nullptr) {
        ledManager_.setBlinking();
        hidTypeString((char*)buffer_);
        memset(buffer_, 0, bufferSize_);
        delay(100);
        ledManager_.setOn();
    }
}
