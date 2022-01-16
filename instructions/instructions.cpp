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

void nes_emu::CPU::adc(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    uint8_t argument = _memory->read_uint8(opAddress);
    adc_impl(argument);
}

void nes_emu::CPU::instrAnd(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    uint8_t argument = _memory->read_uint8(opAddress);

    _registers->accumulator &= argument;
    
    setNumberFlags(_registers->accumulator);
}

void nes_emu::CPU::asl(AddressMode mode) {
    uint8_t value;

    if (mode == AddressMode::Accumulator) {
        value = _registers->accumulator;
    } else {
        uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
        value = _memory->read_uint8(opAddress);
    }

    if (value & 0x80) {
        _registers->statusRegister |= nes_registers::StatusFlags::CarryFlag;
    } else {
        _registers->statusRegister &= ~nes_registers::StatusFlags::CarryFlag;
    }
    
    value = value << 1;
    setNumberFlags(value);
    
    if (mode == AddressMode::Accumulator) {
        _registers->accumulator = value;
    } else {
        uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
        _memory->write(opAddress, value);
    }
}

void nes_emu::CPU::brk() {
}

void nes_emu::CPU::lda(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    auto value = _memory->read_uint8(opAddress);
    _registers->accumulator = value;
    setNumberFlags(_registers->accumulator);
}

void nes_emu::CPU::ldx(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    auto value = _memory->read_uint8(opAddress);
    _registers->x = value;
    setNumberFlags(_registers->x);
}

void nes_emu::CPU::ldy(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    auto value = _memory->read_uint8(opAddress);
    _registers->y = value;
    setNumberFlags(_registers->y);
}

void nes_emu::CPU::sbc(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    uint8_t argument = _memory->read_uint8(opAddress);
    adc_impl(~argument);
}

void nes_emu::CPU::sta(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    _memory->write(opAddress, _registers->accumulator);
}

void nes_emu::CPU::stx(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    _memory->write(opAddress, _registers->x);
}

void nes_emu::CPU::sty(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    _memory->write(opAddress, _registers->y);
}

void nes_emu::CPU::tax() {
    _registers->x = _registers->accumulator;
    setNumberFlags(_registers->x);
}

void nes_emu::CPU::setNumberFlags(uint8_t lastOperationValue) {
    if (lastOperationValue == 0) {
        _registers->statusRegister |= nes_registers::StatusFlags::ZeroFlag;
    } else {
        _registers->statusRegister &= ~nes_registers::StatusFlags::ZeroFlag;
    }
    
    if ((lastOperationValue & 0x80) != 0) {
        _registers->statusRegister |= nes_registers::StatusFlags::NegativeFlag;
    } else {
        _registers->statusRegister &= ~nes_registers::StatusFlags::NegativeFlag;
    }
}

void nes_emu::CPU::adc_impl(uint8_t argument) {
    uint16_t addResult = argument + _registers->accumulator + (_registers->statusRegister & nes_registers::StatusFlags::CarryFlag);

    if (addResult > 0xFF) {
        _registers->statusRegister |= nes_registers::StatusFlags::CarryFlag;
    } else {
        _registers->statusRegister &= ~nes_registers::StatusFlags::CarryFlag;
    }
    
    //overflow flag logic from: https://stackoverflow.com/a/29224684
    if (~(_registers->accumulator ^ argument) & (_registers->accumulator ^ addResult) & 0x80) {
        _registers->statusRegister |= nes_registers::StatusFlags::OverflowFlag;
    } else {
        _registers->statusRegister &= ~nes_registers::StatusFlags::OverflowFlag;
    }
    
    _registers->accumulator = addResult;
    
    setNumberFlags(_registers->accumulator);
}
