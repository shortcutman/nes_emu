//
//  input_controller.cpp
//  nes_emu
//
//  Created by Daniel Burke on 9/5/2026.
//

#include "input_controller.hpp"

namespace {
    const uint8_t ControllerLatchBit = 0x1;
}

void nes_emu::Controller::setUpdateCallback(UpdateCallback callback) {
    _updateCallback = callback;
}

void nes_emu::Controller::writeStrobe(uint8_t byte) {
    if (!(_strobe & ControllerLatchBit) && byte & ControllerLatchBit) {
        _joy1PollPosition = 0;
        _joy2PollPosition = 0;
    }

    _strobe = byte;
}

uint8_t nes_emu::Controller::readJoy1() {
    if (_strobe & ControllerLatchBit) {
        return _updateCallback(0, Button::A) ? 0x1 : 0x0;
    } else if (!(_strobe & ControllerLatchBit) && _joy1PollPosition < Button::MAX) {
        return _updateCallback(0, static_cast<Button>(_joy1PollPosition++)) ? 0x1 : 0x0;
    }

    return 1;
}

uint8_t nes_emu::Controller::readJoy2() {
    if (_strobe & ControllerLatchBit) {
        return _updateCallback(1, Button::A);
    } else if (!(_strobe & ControllerLatchBit) && _joy2PollPosition < Button::MAX) {
        return _updateCallback(1, static_cast<Button>(_joy2PollPosition++)) ? 0x1 : 0x0;
    }

    return 1;
}