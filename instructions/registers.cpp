//
//  registers.cpp
//  nes_emu
//
//  Created by Daniel Burke on 13/1/2022.
//

#include "registers.hpp"

void nes_registers::setStatusFlag(StatusFlags flag, bool value) {
    if (value) {
        this->statusRegister |= flag;
    } else {
        this->statusRegister &= ~flag;
    }
}

bool nes_registers::getStatusFlag(StatusFlags flag) {
    return this->statusRegister & flag;
}
