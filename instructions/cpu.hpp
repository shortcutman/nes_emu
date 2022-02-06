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
#include <string>
#include <unordered_map>
#include <functional>

#include "instruction_utils.hpp"

struct nes_registers;

namespace nes_emu {
    class Memory;

    class CPU {
        friend class CPUTest;
        
        struct OpCode {
            uint8_t code;
            uint8_t bytes;
            uint8_t cycles;
            std::string shortHand;
            std::function<void(void)> func;
        };
        typedef std::unordered_map<uint8_t, OpCode> OpCodeMap;
        static OpCodeMap buildOpCodeMap(CPU* cpu);
        
    protected:
        enum Interrupt : uint16_t {
            None = 0x0000,
            NMI = 0xFFFA,
            Reset = 0xFFFC,
            IRQBRK = 0xFFFE
        };
        
    protected:
        std::unique_ptr<nes_registers> _registers;
        std::unique_ptr<nes_emu::Memory> _memory;
        OpCodeMap _ops;
        Interrupt _interrupt;
        
    public:
        CPU();
        ~CPU();
        
        void executeOne();
        
    protected:
        void aax(AddressMode mode);
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
        
        void dcp(AddressMode mode);
        
        void dec(AddressMode mode);
        void dex();
        void dey();
        
        void dop(AddressMode mode);
        
        void eor(AddressMode mode);
        
        void inc(AddressMode mode);
        void inx();
        void iny();
        
        void isc(AddressMode mode);
        
        void jmp(AddressMode mode);
        void jsr();
        
        void lax(AddressMode mode);
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
        void rla(AddressMode mode);
        void ror(AddressMode mode);
        void sre(AddressMode mode);
        void rra(AddressMode mode);
        
        void rti();
        void rts();
        
        void sbc(AddressMode mode);
        
        void sec();
        void sed();
        void sei();
        
        void slo(AddressMode mode);
        
        void sta(AddressMode mode);
        void stx(AddressMode mode);
        void sty(AddressMode mode);
        
        void tax();
        void tay();
        void tsx();
        void txa();
        void txs();
        void tya();
        
        void top(AddressMode mode);
        
    protected:
        void setNumberFlags(uint8_t lastOperationValue);
        void adc_impl(uint8_t argument);
        void branch_impl(bool test);
        void compare_impl(AddressMode mode, uint8_t registerInput);
        void popStackAndSetStatus();
        
        void stack_push(uint8_t value);
        uint8_t stack_pop();
        
        void stack_pushu16(uint16_t value);
        uint16_t stack_popu16();
        
        bool checkAndSetupInterrupt();
    };
}

#endif /* instructions_hpp */
