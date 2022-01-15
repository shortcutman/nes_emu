//
//  instructions.hpp
//  nes_emu
//
//  Created by Daniel Burke on 13/1/2022.
//

#ifndef instructions_hpp
#define instructions_hpp

#include <cstdint>
#include <memory>

#include "instruction_utils.hpp"

struct nes_registers;
class nes_memory;

namespace nes_emu {

    class CPU {
        friend class CPUTest;
        
    public:
        std::unique_ptr<nes_registers> _registers;
        std::unique_ptr<nes_memory> _memory;
        
    public:
        CPU();
        ~CPU();
        
    protected:
        static unsigned int brk(nes_registers& registers); //opcode 0x00
        static unsigned int lda(AddressMode mode, nes_registers& registers, nes_memory& memory);
        static unsigned int tax(nes_registers& registers); //opcode 0xAA
    };
}

#endif /* instructions_hpp */
