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
        
        void bcc();
        void bcs();
        void beq();
        void bit(AddressMode mode);
        void bmi();
        void bne();
        void bpl();
        void brk(); //opcode 0x00
        void bvc();
        void bvs();
        
        void clc();
        void cld();
        void cli();
        void clv();
        
        void cmp(AddressMode mode);
        void cpx(AddressMode mode);
        void cpy(AddressMode mode);
        
        void dec(AddressMode mode);
        void dex();
        void dey();
        
        void eor(AddressMode mode);
        
        void inc(AddressMode mode);
        void inx();
        void iny();
        
        void jmp(AddressMode mode);
        void jsr();
        
        void lda(AddressMode mode);
        void ldx(AddressMode mode);
        void ldy(AddressMode mode);
        
        void lsr(AddressMode mode);
        
        void nop();
        
        void ora(AddressMode mode);
        
        void pha();
        void php();
        void pla();
        void plp();
        
        void rol(AddressMode mode);
        void ror(AddressMode mode);
        
        void rti();
        void rts();
        
        void sbc(AddressMode mode);
        
        void sec();
        void sed();
        void sei();
        
        void sta(AddressMode mode);
        void stx(AddressMode mode);
        void sty(AddressMode mode);
        
        void tax();
        void tay();
        void tsx();
        void txa();
        void txs();
        void tya();
        
    protected:
        void setNumberFlags(uint8_t lastOperationValue);
        void adc_impl(uint8_t argument);
        void branch_impl(bool test);
        void compare_impl(AddressMode mode, uint8_t registerInput);
        
        void stack_push(uint8_t value);
        uint8_t stack_pop();
        
        void stack_pushu16(uint16_t value);
        uint16_t stack_popu16();
    };
}

#endif /* instructions_hpp */
