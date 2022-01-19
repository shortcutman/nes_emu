//
//  opcodes.cpp
//  instructions
//
//  Created by Daniel Burke on 19/1/2022.
//

#include "instructions.hpp"

nes_emu::CPU::OpCodeMap nes_emu::CPU::buildOpCodeMap(nes_emu::CPU* cpu) {
    return {
        {0x69, {0x69, 2, 2, "ADC", [cpu] { cpu->adc(AddressMode::Immediate); }}},
        {0x65, {0x65, 2, 3, "ADC", [cpu] { cpu->adc(AddressMode::ZeroPage); }}},
        {0x75, {0x75, 2, 4, "ADC", [cpu] { cpu->adc(AddressMode::ZeroPageX); }}},
        {0x6D, {0x6D, 3, 4, "ADC", [cpu] { cpu->adc(AddressMode::Absolute); }}},
        {0x7D, {0x7D, 3, 4, "ADC", [cpu] { cpu->adc(AddressMode::AbsoluteX); }}},
        {0x79, {0x79, 3, 4, "ADC", [cpu] { cpu->adc(AddressMode::AbsoluteY); }}},
        {0x61, {0x61, 2, 6, "ADC", [cpu] { cpu->adc(AddressMode::Indirect_X); }}},
        {0x71, {0x71, 2, 5, "ADC", [cpu] { cpu->adc(AddressMode::Indirect_Y); }}},
        
        {0x29, {0x29, 2, 2, "AND", [cpu] { cpu->instrAnd(AddressMode::Immediate); }}},
        {0x25, {0x25, 2, 3, "AND", [cpu] { cpu->instrAnd(AddressMode::ZeroPage); }}},
        {0x35, {0x35, 2, 4, "AND", [cpu] { cpu->instrAnd(AddressMode::ZeroPageX); }}},
        {0x2D, {0x2D, 3, 4, "AND", [cpu] { cpu->instrAnd(AddressMode::Absolute); }}},
        {0x3D, {0x3D, 3, 4, "AND", [cpu] { cpu->instrAnd(AddressMode::AbsoluteX); }}},
        {0x39, {0x39, 3, 4, "AND", [cpu] { cpu->instrAnd(AddressMode::AbsoluteY); }}},
        {0x21, {0x21, 2, 6, "AND", [cpu] { cpu->instrAnd(AddressMode::Indirect_X); }}},
        {0x31, {0x31, 2, 5, "AND", [cpu] { cpu->instrAnd(AddressMode::Indirect_Y); }}},
        
        {0x0A, {0x0A, 1, 2, "ASL", [cpu] { cpu->asl(AddressMode::Immediate); }}},
        {0x06, {0x06, 2, 5, "ASL", [cpu] { cpu->asl(AddressMode::ZeroPage); }}},
        {0x16, {0x16, 2, 6, "ASL", [cpu] { cpu->asl(AddressMode::ZeroPageX); }}},
        {0x0E, {0x0E, 3, 6, "ASL", [cpu] { cpu->asl(AddressMode::Absolute); }}},
        {0x1E, {0x1E, 3, 7, "ASL", [cpu] { cpu->asl(AddressMode::AbsoluteX); }}},
        
        {0x90, {0x90, 2, 2, "BCC", [cpu] { cpu->bcc(); }}},
        
        {0xB0, {0xB0, 2, 2, "BCS", [cpu] { cpu->bcs(); }}},
        
        {0xF0, {0xF0, 2, 2, "BEQ", [cpu] { cpu->beq(); }}},
        
        {0x24, {0x24, 2, 3, "BIT", [cpu] { cpu->bit(AddressMode::ZeroPage); }}},
        {0x2C, {0x2C, 3, 4, "BIT", [cpu] { cpu->bit(AddressMode::Absolute); }}},
        
        {0x30, {0x30, 2, 2, "BMI", [cpu] { cpu->bmi(); }}},
        
        {0xD0, {0xD0, 2, 2, "BNE", [cpu] { cpu->bne(); }}},
        
        {0x10, {0x10, 2, 2, "BPL", [cpu] { cpu->bpl(); }}},

        {0x00, {0x00, 1, 7, "BRK", [cpu] { cpu->brk(); }}},

        {0x50, {0x50, 2, 2, "BVC", [cpu] { cpu->bvc(); }}},
        
        {0x70, {0x70, 2, 2, "BVS", [cpu] { cpu->bvs(); }}},
        
        {0x18, {0x18, 1, 2, "CLC", [cpu] { cpu->clc(); }}},
        
        {0xD8, {0xD8, 1, 2, "CLD", [cpu] { cpu->cld(); }}},
        
        {0x58, {0x58, 1, 2, "CLI", [cpu] { cpu->cli(); }}},
        
        {0xB8, {0xB8, 1, 2, "CLV", [cpu] { cpu->clv(); }}},
        
        {0xC9, {0xC9, 2, 2, "CMP", [cpu] { cpu->cmp(AddressMode::Immediate); }}},
        {0xC5, {0xC5, 2, 3, "CMP", [cpu] { cpu->cmp(AddressMode::ZeroPage); }}},
        {0xD5, {0xD5, 2, 4, "CMP", [cpu] { cpu->cmp(AddressMode::ZeroPageX); }}},
        {0xCD, {0xCD, 3, 4, "CMP", [cpu] { cpu->cmp(AddressMode::Absolute); }}},
        {0xDD, {0xDD, 3, 4, "CMP", [cpu] { cpu->cmp(AddressMode::AbsoluteX); }}},
        {0xD9, {0xD9, 3, 4, "CMP", [cpu] { cpu->cmp(AddressMode::AbsoluteY); }}},
        {0xC1, {0xC1, 2, 6, "CMP", [cpu] { cpu->cmp(AddressMode::Indirect_X); }}},
        {0xD1, {0xD1, 2, 5, "CMP", [cpu] { cpu->cmp(AddressMode::Indirect_Y); }}},
        
        {0xE0, {0xE0, 2, 2, "CPX", [cpu] { cpu->cpx(AddressMode::Immediate); }}},
        {0xE4, {0xE4, 2, 3, "CPX", [cpu] { cpu->cpx(AddressMode::ZeroPage); }}},
        {0xEC, {0xEC, 3, 4, "CPX", [cpu] { cpu->cpx(AddressMode::Absolute); }}},
        
        {0xC0, {0xC0, 2, 2, "CPY", [cpu] { cpu->cpy(AddressMode::Immediate); }}},
        {0xC4, {0xC4, 2, 3, "CPY", [cpu] { cpu->cpy(AddressMode::ZeroPage); }}},
        {0xCC, {0xCC, 3, 4, "CPY", [cpu] { cpu->cpy(AddressMode::Absolute); }}},
        
        {0xC6, {0xC6, 2, 5, "DEC", [cpu] { cpu->dec(AddressMode::ZeroPage); }}},
        {0xD6, {0xD6, 2, 6, "DEC", [cpu] { cpu->dec(AddressMode::ZeroPageX); }}},
        {0xCE, {0xCE, 3, 6, "DEC", [cpu] { cpu->dec(AddressMode::Absolute); }}},
        {0xDE, {0xDE, 3, 7, "DEC", [cpu] { cpu->dec(AddressMode::AbsoluteX); }}},

        {0xCA, {0xCA, 1, 2, "DEX", [cpu] { cpu->dex(); }}},
        
        {0x88, {0x88, 1, 2, "DEY", [cpu] { cpu->dey(); }}},
        
        {0x49, {0x49, 2, 2, "EOR", [cpu] { cpu->eor(AddressMode::Immediate); }}},
        {0x45, {0x45, 2, 3, "EOR", [cpu] { cpu->eor(AddressMode::ZeroPage); }}},
        {0x55, {0x55, 2, 4, "EOR", [cpu] { cpu->eor(AddressMode::ZeroPageX); }}},
        {0x4D, {0x4D, 3, 4, "EOR", [cpu] { cpu->eor(AddressMode::Absolute); }}},
        {0x5D, {0x5D, 3, 4, "EOR", [cpu] { cpu->eor(AddressMode::AbsoluteX); }}},
        {0x59, {0x59, 3, 4, "EOR", [cpu] { cpu->eor(AddressMode::AbsoluteY); }}},
        {0x41, {0x41, 2, 6, "EOR", [cpu] { cpu->eor(AddressMode::Indirect_X); }}},
        {0x51, {0x51, 2, 5, "EOR", [cpu] { cpu->eor(AddressMode::Indirect_Y); }}},
        
        {0xE6, {0xE6, 2, 5, "INC", [cpu] { cpu->inc(AddressMode::ZeroPage); }}},
        {0xF6, {0xF6, 2, 6, "INC", [cpu] { cpu->inc(AddressMode::ZeroPageX); }}},
        {0xEE, {0xEE, 3, 6, "INC", [cpu] { cpu->inc(AddressMode::Absolute); }}},
        {0xFE, {0xFE, 3, 7, "INC", [cpu] { cpu->inc(AddressMode::AbsoluteX); }}},
        
        {0xE8, {0xE8, 1, 2, "INX", [cpu] { cpu->inx(); }}},
        
        {0xC8, {0xC8, 1, 2, "INY", [cpu] { cpu->iny(); }}},
        
        {0x4C, {0x4C, 3, 3, "JMP", [cpu] { cpu->jmp(AddressMode::Absolute); }}},
        {0x6C, {0x6C, 3, 5, "JMP", [cpu] { cpu->jmp(AddressMode::Indirect); }}},

        {0x20, {0x20, 3, 6, "JSR", [cpu] { cpu->jsr(); }}},

        {0xA9, {0xA9, 2, 2, "LDA", [cpu] { cpu->lda(AddressMode::Immediate); }}},
        {0xA5, {0xA5, 2, 3, "LDA", [cpu] { cpu->lda(AddressMode::ZeroPage); }}},
        {0xB5, {0xB5, 2, 4, "LDA", [cpu] { cpu->lda(AddressMode::ZeroPageX); }}},
        {0xAD, {0xAD, 3, 4, "LDA", [cpu] { cpu->lda(AddressMode::Absolute); }}},
        {0xBD, {0xBD, 3, 4, "LDA", [cpu] { cpu->lda(AddressMode::AbsoluteX); }}},
        {0xB9, {0xB9, 3, 4, "LDA", [cpu] { cpu->lda(AddressMode::AbsoluteY); }}},
        {0xA1, {0xA1, 2, 6, "LDA", [cpu] { cpu->lda(AddressMode::Indirect_X); }}},
        {0xB1, {0xB1, 2, 5, "LDA", [cpu] { cpu->lda(AddressMode::Indirect_Y); }}},
        
        {0xA2, {0xA2, 2, 2, "LDX", [cpu] { cpu->ldx(AddressMode::Immediate); }}},
        {0xA6, {0xA6, 2, 3, "LDX", [cpu] { cpu->ldx(AddressMode::ZeroPage); }}},
        {0xB6, {0xB6, 2, 4, "LDX", [cpu] { cpu->ldx(AddressMode::ZeroPageY); }}},
        {0xAE, {0xAE, 3, 4, "LDX", [cpu] { cpu->ldx(AddressMode::Absolute); }}},
        {0xBE, {0xBE, 3, 4, "LDX", [cpu] { cpu->ldx(AddressMode::AbsoluteY); }}},
        
        {0xA0, {0xA0, 2, 2, "LDY", [cpu] { cpu->ldy(AddressMode::Immediate); }}},
        {0xA4, {0xA4, 2, 3, "LDY", [cpu] { cpu->ldy(AddressMode::ZeroPage); }}},
        {0xB4, {0xB4, 2, 4, "LDY", [cpu] { cpu->ldy(AddressMode::ZeroPageX); }}},
        {0xAC, {0xAC, 3, 4, "LDY", [cpu] { cpu->ldy(AddressMode::Absolute); }}},
        {0xBC, {0xBC, 3, 4, "LDY", [cpu] { cpu->ldy(AddressMode::AbsoluteX); }}},
        
        {0x4A, {0x4A, 1, 2, "LSR", [cpu] { cpu->lsr(AddressMode::Accumulator); }}},
        {0x46, {0x46, 2, 5, "LSR", [cpu] { cpu->lsr(AddressMode::ZeroPage); }}},
        {0x56, {0x56, 2, 6, "LSR", [cpu] { cpu->lsr(AddressMode::ZeroPageX); }}},
        {0x4E, {0x4E, 3, 6, "LSR", [cpu] { cpu->lsr(AddressMode::Absolute); }}},
        {0x5E, {0x5E, 3, 7, "LSR", [cpu] { cpu->lsr(AddressMode::AbsoluteX); }}},
        
        {0xEA, {0xEA, 1, 2, "NOP", [cpu] { cpu->nop(); }}},

        {0x09, {0x09, 2, 2, "ORA", [cpu] { cpu->ora(AddressMode::Immediate); }}},
        {0x05, {0x05, 2, 3, "ORA", [cpu] { cpu->ora(AddressMode::ZeroPage); }}},
        {0x15, {0x15, 2, 4, "ORA", [cpu] { cpu->ora(AddressMode::ZeroPageX); }}},
        {0x0D, {0x0D, 3, 4, "ORA", [cpu] { cpu->ora(AddressMode::Absolute); }}},
        {0x1D, {0x1D, 3, 4, "ORA", [cpu] { cpu->ora(AddressMode::AbsoluteX); }}},
        {0x19, {0x19, 3, 4, "ORA", [cpu] { cpu->ora(AddressMode::AbsoluteY); }}},
        {0x01, {0x01, 2, 6, "ORA", [cpu] { cpu->ora(AddressMode::Indirect_X); }}},
        {0x11, {0x11, 2, 5, "ORA", [cpu] { cpu->ora(AddressMode::Indirect_Y); }}},
        
        {0x48, {0x48, 1, 3, "PHA", [cpu] { cpu->pha(); }}},
        
        {0x08, {0x08, 1, 3, "PHP", [cpu] { cpu->php(); }}},
        
        {0x68, {0x68, 1, 4, "PLA", [cpu] { cpu->pla(); }}},
        
        {0x28, {0x28, 1, 4, "PLP", [cpu] { cpu->plp(); }}},
        
        {0x2A, {0x2A, 1, 2, "ROL", [cpu] { cpu->rol(AddressMode::Accumulator); }}},
        {0x26, {0x26, 2, 5, "ROL", [cpu] { cpu->rol(AddressMode::ZeroPage); }}},
        {0x36, {0x36, 2, 6, "ROL", [cpu] { cpu->rol(AddressMode::ZeroPageX); }}},
        {0x2E, {0x2E, 3, 6, "ROL", [cpu] { cpu->rol(AddressMode::Absolute); }}},
        {0x3E, {0x3E, 3, 7, "ROL", [cpu] { cpu->rol(AddressMode::AbsoluteX); }}},
        
        {0x6A, {0x6A, 1, 2, "ROR", [cpu] { cpu->ror(AddressMode::Accumulator); }}},
        {0x66, {0x66, 2, 5, "ROR", [cpu] { cpu->ror(AddressMode::ZeroPage); }}},
        {0x76, {0x76, 2, 6, "ROR", [cpu] { cpu->ror(AddressMode::ZeroPageX); }}},
        {0x6E, {0x6E, 3, 6, "ROR", [cpu] { cpu->ror(AddressMode::Absolute); }}},
        {0x7E, {0x7E, 3, 7, "ROR", [cpu] { cpu->ror(AddressMode::AbsoluteX); }}},
        
        {0x40, {0x40, 1, 6, "RTI", [cpu] { cpu->rti(); }}},
        
        {0x60, {0x60, 1, 6, "RTS", [cpu] { cpu->rts(); }}},
        
        {0xE9, {0xE9, 2, 2, "SBC", [cpu] { cpu->sbc(AddressMode::Immediate); }}},
        {0xE5, {0xE5, 2, 3, "SBC", [cpu] { cpu->sbc(AddressMode::ZeroPage); }}},
        {0xF5, {0xF5, 2, 4, "SBC", [cpu] { cpu->sbc(AddressMode::ZeroPageX); }}},
        {0xED, {0xED, 3, 4, "SBC", [cpu] { cpu->sbc(AddressMode::Absolute); }}},
        {0xFD, {0xFD, 3, 4, "SBC", [cpu] { cpu->sbc(AddressMode::AbsoluteX); }}},
        {0xF9, {0xF9, 3, 4, "SBC", [cpu] { cpu->sbc(AddressMode::AbsoluteY); }}},
        {0xE1, {0xE1, 2, 6, "SBC", [cpu] { cpu->sbc(AddressMode::Indirect_X); }}},
        {0xF1, {0xF1, 2, 5, "SBC", [cpu] { cpu->sbc(AddressMode::Indirect_Y); }}},
        
        {0x38, {0x38, 1, 2, "SEC", [cpu] { cpu->sec(); }}},
        
        {0xF8, {0xF8, 1, 2, "SED", [cpu] { cpu->sed(); }}},
        
        {0x78, {0x78, 1, 2, "SEI", [cpu] { cpu->sei(); }}},
        
        {0x85, {0x85, 2, 3, "STA", [cpu] { cpu->sta(AddressMode::ZeroPage); }}},
        {0x95, {0x95, 2, 4, "STA", [cpu] { cpu->sta(AddressMode::ZeroPageX); }}},
        {0x8D, {0x8D, 3, 4, "STA", [cpu] { cpu->sta(AddressMode::Absolute); }}},
        {0x9D, {0x9D, 3, 5, "STA", [cpu] { cpu->sta(AddressMode::AbsoluteX); }}},
        {0x99, {0x99, 3, 5, "STA", [cpu] { cpu->sta(AddressMode::AbsoluteY); }}},
        {0x81, {0x81, 2, 6, "STA", [cpu] { cpu->sta(AddressMode::Indirect_X); }}},
        {0x91, {0x91, 2, 5, "STA", [cpu] { cpu->sta(AddressMode::Indirect_Y); }}},
        
        {0x86, {0x86, 2, 3, "STX", [cpu] { cpu->stx(AddressMode::ZeroPage); }}},
        {0x96, {0x96, 2, 4, "STX", [cpu] { cpu->stx(AddressMode::ZeroPageY); }}},
        {0x8E, {0x8E, 3, 4, "STX", [cpu] { cpu->stx(AddressMode::Absolute); }}},
        
        {0x84, {0x84, 2, 3, "STY", [cpu] { cpu->sty(AddressMode::ZeroPage); }}},
        {0x94, {0x94, 2, 4, "STY", [cpu] { cpu->sty(AddressMode::ZeroPageX); }}},
        {0x8C, {0x8C, 3, 4, "STY", [cpu] { cpu->sty(AddressMode::Absolute); }}},
        
        {0xAA, {0xAA, 1, 2, "TAX", [cpu] { cpu->tax(); }}},
        
        {0xA8, {0xA8, 1, 2, "TAY", [cpu] { cpu->tay(); }}},

        {0xBA, {0xBA, 1, 2, "TSX", [cpu] { cpu->tsx(); }}},
        
        {0x8A, {0x8A, 1, 2, "TXA", [cpu] { cpu->txa(); }}},
        
        {0x9A, {0x9A, 1, 2, "TXS", [cpu] { cpu->txs(); }}},
        
        {0x98, {0x98, 1, 2, "TYA", [cpu] { cpu->tya(); }}},
    };
}
