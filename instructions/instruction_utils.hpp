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

namespace nes_emu {
    class Memory;

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
};

bool pageCrossAdvanceClock(uint16_t addrA, uint16_t addrB, nes_emu::Memory& memory);

uint16_t decodeOperandAddress(AddressMode mode, nes_registers& registers, nes_emu::Memory& memory, bool advanceOnlyOnPageCross = true);
}

#endif /* instruction_utils_hpp */
