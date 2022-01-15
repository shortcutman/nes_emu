//
//  instruction_utils.hpp
//  instructions
//
//  Created by Daniel Burke on 15/1/2022.
//

#ifndef instruction_utils_hpp
#define instruction_utils_hpp

#include <cstdint>

struct nes_registers;
class nes_memory;

namespace nes_emu {

enum AddressMode {
    Absolute,
    AbsoluteX,
    AbsoluteY,
    ZeroPage,
    ZeroPageX,
    ZeroPageY,
    Immediate,
    IndexedIndirect,
    IndirectIndexed,
    Relative
};

uint16_t decodeOperandAddress(AddressMode mode, nes_registers& registers, const nes_memory& memory);

uint8_t statusFlagsOnByteValue(const uint8_t value, const uint8_t currentStatus);

}

#endif /* instruction_utils_hpp */