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

unsigned int nes_emu::CPU::ldx(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    auto value = _memory->read_uint8(opAddress);
    _registers->x = value;
    _registers->statusRegister = statusFlagsOnByteValue(_registers->x, _registers->statusRegister);
    
    return 2;
}

unsigned int nes_emu::CPU::ldy(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    auto value = _memory->read_uint8(opAddress);
    _registers->y = value;
    _registers->statusRegister = statusFlagsOnByteValue(_registers->y, _registers->statusRegister);
    
    return 2;
}

unsigned int nes_emu::CPU::sta(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    _memory->write(opAddress, _registers->accumulator);
    
    return 2;
}

unsigned int nes_emu::CPU::stx(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    _memory->write(opAddress, _registers->x);
    
    return 2;
}

unsigned int nes_emu::CPU::sty(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    _memory->write(opAddress, _registers->y);
    
    return 2;
}

unsigned int nes_emu::CPU::tax() {
    _registers->x = _registers->accumulator;
    _registers->statusRegister = statusFlagsOnByteValue(_registers->x, _registers->statusRegister);
    
    return 2;
}
