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

void nes_emu::CPU::bcc() {
    branch_impl(!(_registers->statusRegister & nes_registers::StatusFlags::CarryFlag));
}

void nes_emu::CPU::bcs() {
    branch_impl(_registers->statusRegister & nes_registers::StatusFlags::CarryFlag);
}

void nes_emu::CPU::beq() {
    branch_impl(_registers->statusRegister & nes_registers::StatusFlags::ZeroFlag);
}

void nes_emu::CPU::bit(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    uint8_t value  = _memory->read_uint8(opAddress);
    
    if (value & _registers->accumulator) {
        _registers->statusRegister &= ~nes_registers::StatusFlags::ZeroFlag;
    } else {
        _registers->statusRegister |= nes_registers::StatusFlags::ZeroFlag;
    }
    
    _registers->setStatusFlag(nes_registers::StatusFlags::NegativeFlag, value & 0b10000000);
    _registers->setStatusFlag(nes_registers::StatusFlags::OverflowFlag, value & 0b01000000);
}

void nes_emu::CPU::bmi() {
    branch_impl(_registers->statusRegister & nes_registers::StatusFlags::NegativeFlag);
}

void nes_emu::CPU::bne() {
    branch_impl(!(_registers->statusRegister & nes_registers::StatusFlags::ZeroFlag));
}

void nes_emu::CPU::bpl() {
    branch_impl(!(_registers->statusRegister & nes_registers::StatusFlags::NegativeFlag));
}

void nes_emu::CPU::brk() {
    throw std::runtime_error("Not implemented");
}

void nes_emu::CPU::bvc() {
    branch_impl(!(_registers->statusRegister & nes_registers::StatusFlags::OverflowFlag));
}

void nes_emu::CPU::bvs() {
    branch_impl(_registers->statusRegister & nes_registers::StatusFlags::OverflowFlag);
}

void nes_emu::CPU::clc() {
    _registers->setStatusFlag(nes_registers::StatusFlags::CarryFlag, false);
}

void nes_emu::CPU::cld() {
    _registers->setStatusFlag(nes_registers::StatusFlags::DecimalModeFlag, false);
}

void nes_emu::CPU::clv() {
    _registers->setStatusFlag(nes_registers::StatusFlags::OverflowFlag, false);
}

void nes_emu::CPU::cmp(AddressMode mode) {
    compare_impl(mode, _registers->accumulator);
}

void nes_emu::CPU::cpx(AddressMode mode) {
    compare_impl(mode, _registers->x);
}

void nes_emu::CPU::cpy(AddressMode mode) {
    compare_impl(mode, _registers->y);
}

void nes_emu::CPU::dec(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    auto value = _memory->read_uint8(opAddress) - 1;
    _memory->write(opAddress, value);
    setNumberFlags(value);
}

void nes_emu::CPU::dex() {
    _registers->x -= 1;
    setNumberFlags(_registers->x);
}

void nes_emu::CPU::dey() {
    _registers->y -= 1;
    setNumberFlags(_registers->y);
}

void nes_emu::CPU::eor(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    auto value = _memory->read_uint8(opAddress);

    _registers->accumulator ^= value;
    setNumberFlags(_registers->accumulator);
}

void nes_emu::CPU::inc(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    auto value = _memory->read_uint8(opAddress) + 1;
    _memory->write(opAddress, value);
    setNumberFlags(value);
}

void nes_emu::CPU::inx() {
    _registers->x += 1;
    setNumberFlags(_registers->x);
}

void nes_emu::CPU::iny() {
    _registers->y += 1;
    setNumberFlags(_registers->y);
}

void nes_emu::CPU::jmp(AddressMode mode) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    _registers->programCounter = opAddress;
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

void nes_emu::CPU::branch_impl(bool test) {
    auto incrementValue = _memory->read_uint8(_registers->programCounter);
    _registers->programCounter++;
    
    if (test) {
        _registers->programCounter += incrementValue;
    }
}

void nes_emu::CPU::compare_impl(AddressMode mode, uint8_t registerInput) {
    uint16_t opAddress = decodeOperandAddress(mode, *_registers, *_memory);
    auto value = _memory->read_uint8(opAddress);
    
    uint8_t result = registerInput - value;
    _registers->setStatusFlag(nes_registers::StatusFlags::CarryFlag, registerInput >= value);
    _registers->setStatusFlag(nes_registers::StatusFlags::ZeroFlag, registerInput == value);
    _registers->setStatusFlag(nes_registers::StatusFlags::NegativeFlag, result & 0x80);
}
