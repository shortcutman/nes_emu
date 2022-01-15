//
//  instructions.hpp
//  nes_emu
//
//  Created by Daniel Burke on 13/1/2022.
//

#ifndef instructions_hpp
#define instructions_hpp

#include <cstdint>

#include "instruction_utils.hpp"

struct nes_registers;
class nes_memory;

namespace nes_emu {

unsigned int brk(nes_registers& registers); //opcode 0x00

unsigned int lda(AddressMode mode, nes_registers& registers, nes_memory& memory); //opcode 0xA5

unsigned int tax(nes_registers& registers); //opcode 0xAA
}

#endif /* instructions_hpp */
