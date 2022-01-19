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

enum class AddressMode : uint8_t {
    Accumulator,
    Absolute,
    AbsoluteX,
    AbsoluteY,
    ZeroPage,
    ZeroPageX,
    ZeroPageY,
    Immediate,
    Indirect_X,
    Indirect_Y,
    Indirect, //used by jmp
    Relative
};

uint16_t decodeOperandAddress(AddressMode mode, nes_registers& registers, const nes_memory& memory);
}

#endif /* instruction_utils_hpp */
