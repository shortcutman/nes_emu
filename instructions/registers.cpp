//
//  registers.cpp
//  nes_emu
//
//  Created by Daniel Burke on 13/1/2022.
//

#include "registers.hpp"

nes_registers::nes_registers() :
    sp(0xFD),
    p(0x24),
    a(0x00),
    x(0x00),
    y(0x00)
{
    
}

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
