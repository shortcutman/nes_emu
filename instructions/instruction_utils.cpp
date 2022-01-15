//
//  instruction_utils.cpp
//  instructions
//
//  Created by Daniel Burke on 15/1/2022.
//

#include "instruction_utils.hpp"

#include "registers.hpp"
#include "memory.hpp"

namespace {

uint16_t readAndIncrementu16(nes_registers& registers, const nes_memory& memory, uint16_t address) {
    registers.programCounter += 2;
    return memory.read_uint16(address);
}

}

uint16_t nes_emu::decodeOperandAddress(AddressMode mode,
                                       nes_registers& registers,
                                       const nes_memory& memory) {
    switch (mode) {
        case AddressMode::Absolute:
            return readAndIncrementu16(registers, memory, registers.programCounter);
            break;
            
        case AddressMode::AbsoluteX:
            return readAndIncrementu16(registers, memory, registers.programCounter + registers.x);
            break;
            
        case AddressMode::AbsoluteY:
            return readAndIncrementu16(registers, memory, registers.programCounter + registers.y);
            break;
            
        case AddressMode::ZeroPage:
        {
            auto pc = registers.programCounter;
            registers.programCounter += 1;
            return memory.read_uint8(pc);

        }
            break;
            
        case AddressMode::ZeroPageX:
            return memory.read_uint8(registers.programCounter) + registers.x;
            break;
            
        case AddressMode::ZeroPageY:
            return memory.read_uint8(registers.programCounter) + registers.y;
            break;
            
        case AddressMode::Immediate:
        {
            auto pc = registers.programCounter;
            registers.programCounter += 1;
            return pc;
        }
            break;
            
        case AddressMode::IndexedIndirect:
            throw std::runtime_error("unimplemented");
            return 0xDB;
            break;
            
        case AddressMode::IndirectIndexed:
            throw std::runtime_error("unimplemented");
            return 0xDB;
            break;
            
        case AddressMode::Relative:
            throw std::runtime_error("unimplemented");
            return 0xDB;
            break;
    }
}

uint8_t nes_emu::statusFlagsOnByteValue(const uint8_t value, const uint8_t currentStatus) {
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
