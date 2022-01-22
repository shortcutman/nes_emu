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

uint16_t readAndIncrementu16(nes_registers& registers, const nes_emu::Memory& memory, uint16_t address) {
    registers.programCounter += 2;
    return memory.read_uint16(address);
}

uint16_t readAndIncrementu8(nes_registers& registers, const nes_emu::Memory& memory, uint8_t address) {
    registers.programCounter += 1;
    return memory.read_uint8(address);
}

}

uint16_t nes_emu::decodeOperandAddress(AddressMode mode,
                                       nes_registers& registers,
                                       const nes_emu::Memory& memory) {
    switch (mode) {
        case AddressMode::Accumulator:
            throw std::runtime_error("Accumulator address mode does not get values from memory");
            return 0xFFFF;
            break;
            
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
            return readAndIncrementu8(registers, memory, registers.programCounter);
            break;
            
        case AddressMode::ZeroPageX:
            return readAndIncrementu8(registers, memory, static_cast<uint8_t>(registers.programCounter) + registers.x);
            break;
            
        case AddressMode::ZeroPageY:
            return readAndIncrementu8(registers, memory, static_cast<uint8_t>(registers.programCounter) + registers.y);
            break;
            
        case AddressMode::Immediate:
            return registers.programCounter++;
            break;
            
        case AddressMode::Indirect_X:
            throw std::runtime_error("Unimplemented");
            return 0xDB;
            break;
            
        case AddressMode::Indirect_Y:
            throw std::runtime_error("Unimplemented");
            return 0xDB;
            break;
            
        case AddressMode::Indirect:
            auto addrLoc = readAndIncrementu16(registers, memory, registers.programCounter);
            return readAndIncrementu16(registers, memory, addrLoc);
            break;
    }
}
