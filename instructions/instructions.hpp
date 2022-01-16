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
        void adc(AddressMode mode);
        
        void instrAnd(AddressMode mode);
        void asl(AddressMode mode);
        
        void brk(); //opcode 0x00
        
        void lda(AddressMode mode);
        void ldx(AddressMode mode);
        void ldy(AddressMode mode);
        
        void sbc(AddressMode mode);
        
        void sta(AddressMode mode);
        void stx(AddressMode mode);
        void sty(AddressMode mode);
        
        void tax(); //opcode 0xAA
        
    protected:
        void setNumberFlags(uint8_t lastOperationValue);
        void adc_impl(uint8_t argument);
    };
}

#endif /* instructions_hpp */
