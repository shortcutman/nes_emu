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

unsigned int nes_emu::CPU::brk(nes_registers& registers) {
    return 7;
}

unsigned int nes_emu::CPU::lda(AddressMode mode, nes_registers& registers, nes_memory& memory) {
    uint16_t opAddress = decodeOperandAddress(mode, registers, memory);
    auto value = memory.read_uint8(opAddress);
    registers.accumulator = value;
    registers.statusRegister = statusFlagsOnByteValue(registers.accumulator, registers.statusRegister);
    
    return 2;
}

unsigned int nes_emu::CPU::tax(nes_registers &registers) {
    registers.x = registers.accumulator;
    registers.statusRegister = statusFlagsOnByteValue(registers.x, registers.statusRegister);
    
    return 2;
}
