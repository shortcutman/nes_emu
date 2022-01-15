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

namespace {

uint8_t statusFlagsOnByteValue(const uint8_t value, const uint8_t currentStatus) {
    uint8_t updatedStatus = currentStatus;
    
    if (value == 0) {
        updatedStatus |= 0b00000010;
    } else {
        updatedStatus &= 0b11111101;
    }
    
    if ((value & 0b10000000) != 0) {
        updatedStatus |= 0b10000000;
    } else {
        updatedStatus &= 0b01111111;
    }
    
    return updatedStatus;
}

}

unsigned int nes_emu::brk(nes_registers& registers) {
    return 7;
}

unsigned int nes_emu::lda_immediate(nes_registers& registers, nes_memory& memory) {
    uint16_t opAddress = decodeOperandAddress(AddressMode::Immediate, registers, memory);
    auto value = memory.read_uint8(opAddress);
    lda_base(registers, value);
    return 2;
}

unsigned int nes_emu::lda_zero(nes_registers& registers, nes_memory& memory) {
    uint16_t opAddress = decodeOperandAddress(AddressMode::ZeroPage, registers, memory);
    auto value = memory.read_uint8(opAddress);
    lda_base(registers, value);
    return 2;
}

unsigned int nes_emu::lda_base(nes_registers& registers, uint16_t value) {
    registers.accumulator = value;
    registers.statusRegister = statusFlagsOnByteValue(registers.accumulator, registers.statusRegister);
    
    return 2;
}

unsigned int nes_emu::tax(nes_registers &registers) {
    registers.x = registers.accumulator;
    registers.statusRegister = statusFlagsOnByteValue(registers.x, registers.statusRegister);
    
    return 2;
}
