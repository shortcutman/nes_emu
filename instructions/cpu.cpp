//
//  cpu.cpp
//  instructions
//
//  Created by Daniel Burke on 15/1/2022.
//

#include "cpu.hpp"

#include "registers.hpp"
#include "memory.hpp"

uint16_t nes_emu::decodeOperandAddress(AddressMode mode,
                                       nes_registers& registers,
                                       const nes_memory& memory) {
    switch (mode) {
        case AddressMode::Absolute:
            return memory.read_uint16(registers.programCounter);
            break;
            
        case AddressMode::AbsoluteX:
            return memory.read_uint16(registers.programCounter + registers.x);
            break;
            
        case AddressMode::AbsoluteY:
            return memory.read_uint16(registers.programCounter + registers.y);
            break;
            
        case AddressMode::ZeroPage:
            return memory.read_uint8(registers.programCounter);
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
