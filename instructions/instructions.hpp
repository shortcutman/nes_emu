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
        unsigned int adc(AddressMode mode);
        
        unsigned int instrAnd(AddressMode mode);
        
        unsigned int brk(); //opcode 0x00
        
        unsigned int lda(AddressMode mode);
        unsigned int ldx(AddressMode mode);
        unsigned int ldy(AddressMode mode);
        
        unsigned int sbc(AddressMode mode);
        
        unsigned int sta(AddressMode mode);
        unsigned int stx(AddressMode mode);
        unsigned int sty(AddressMode mode);
        
        unsigned int tax(); //opcode 0xAA
        
    private:
        void adc_impl(uint8_t argument);
    };
}

#endif /* instructions_hpp */
