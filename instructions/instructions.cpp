//
//  instructions.cpp
//  nes_emu
//
//  Created by Daniel Burke on 13/1/2022.
//

#include "instructions.hpp"

#include "registers.hpp"
#include "memory.hpp"

#include "cpu.hpp"

nes_emu::CPU::CPU() :
    _registers(new nes_registers()),
    _memory(new nes_memory())
{
}

nes_emu::CPU::~CPU() {
}

unsigned int nes_emu::CPU::brk() {
    return 7;
}

unsigned int nes_emu::CPU::lda(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    auto value = _memory->read_uint8(opAddress);
    _registers->accumulator = value;
    _registers->statusRegister = statusFlagsOnByteValue(_registers->accumulator, _registers->statusRegister);
    
    return 2;
}

unsigned int nes_emu::CPU::tax() {
    _registers->x = _registers->accumulator;
    _registers->statusRegister = statusFlagsOnByteValue(_registers->x, _registers->statusRegister);
    
    return 2;
}
