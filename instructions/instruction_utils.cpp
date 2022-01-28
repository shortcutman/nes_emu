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

uint16_t readAndIncrementu8(nes_registers& registers, const nes_emu::Memory& memory, uint16_t address) {
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
        {
            uint8_t base = readAndIncrementu8(registers, memory, registers.programCounter);
            auto pointer = base + registers.x;
            auto lo = memory.read_uint8(pointer & 0xFF);
            auto hi = memory.read_uint8((pointer + 1) & 0xFF);
            uint16_t address = hi;
            address = address << 8;
            address += lo;
            return address;
        }
            break;
            
        case AddressMode::Indirect_Y:
        {
            uint8_t base = readAndIncrementu8(registers, memory, registers.programCounter);
            uint8_t lo = memory.read_uint8(base);
            uint8_t hi = memory.read_uint8((base + 1) & 0xFF);
            uint16_t deref_base = hi;
            deref_base = deref_base << 8;
            deref_base += lo;
            uint16_t deref = deref_base + registers.y;
            return deref;
        }
            break;
            
        case AddressMode::Indirect:
            auto addrLoc = readAndIncrementu16(registers, memory, registers.programCounter);
            return readAndIncrementu16(registers, memory, addrLoc);
            break;
    }
}
