#include "TouchHandler.h"
#include "keyboard/HidKeyboard.h"
#include <cstring>


TouchHandler::TouchHandler(InternalState &state, LedManager &led, CommandProcessor &cmdProcessor)
    : internalState_(state), ledManager_(led), commandProcessor_(cmdProcessor) {}

void TouchHandler::onSingleTouch() {
    switch (internalState_) {
        case IDLE:
            ledManager_.showNextColor();
            break;
        case PASSWORD_READY:
            internalState_ = TYPING;
            typePassword();
            internalState_ = IDLE;
            break;
        default:
            break;
    }
}

void TouchHandler::onLongTouchStart() {
    if (internalState_ == IDLE) {
        internalState_ = TOUCHING;
        ledManager_.setFadeOutOnce(2);
    }
}

void TouchHandler::onLongTouchEnd() {
    if (internalState_ == TOUCHING) {
        if (commandProcessor_.deriveDefaultPassword()) {
            internalState_ = TYPING;
            typePassword();
        }
        else {
            commandProcessor_.clearOutputBuffer();
        }
        internalState_ = IDLE;
    }
}

void TouchHandler::onLongTouchCancelled() {
    if (internalState_ == TOUCHING) {
        internalState_ = IDLE;
        ledManager_.setOn();
    }
}

void TouchHandler::typePassword() {
    ledManager_.setBlinking();
    hidTypeString((char *)commandProcessor_.getOutputBuffer());
    commandProcessor_.clearOutputBuffer();
    delay(100);
    ledManager_.setOn();
}
