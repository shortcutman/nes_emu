//
//  lda_test.cpp
//  nes_emu-tests
//
//  Created by Daniel Burke on 15/1/2022.
//

#include <gtest/gtest.h>

#include "instructions.hpp"
#include "registers.hpp"
#include "memory.hpp"

namespace {

class CPUTest : public nes_emu::CPU, public ::testing::Test {
};

TEST_F(CPUTest, ADC_PositiveInteger) {
    _memory->write(0x00, 0x69);
    _memory->write(0x01, 0x05);
    _memory->write(0x02, 0x00);
    
    _registers->programCounter = 0x01;
    _registers->statusRegister = 0x00;
    _registers->accumulator = 0x05;
    
    adc(nes_emu::AddressMode::Immediate);
    
    EXPECT_EQ(_registers->programCounter, 0x02);
    EXPECT_EQ(_registers->accumulator, 0x0A);
    EXPECT_EQ(_registers->statusRegister, 0b00000000);
}

TEST_F(CPUTest, ADC_Carry) {
    _memory->write(0x00, 0x69);
    _memory->write(0x01, 0x05);
    _memory->write(0x02, 0x00);
    
    _registers->programCounter = 0x01;
    _registers->statusRegister = 0x00;
    _registers->accumulator = 0xFD;
    
    adc(nes_emu::AddressMode::Immediate);
    
    EXPECT_EQ(_registers->programCounter, 0x02);
    EXPECT_EQ(_registers->accumulator, 0x02);
    EXPECT_EQ(_registers->statusRegister, nes_registers::StatusFlags::CarryFlag);
}

TEST_F(CPUTest, ADC_PositiveOverflow) {
    _memory->write(0x00, 0x69);
    _memory->write(0x01, 0x7F);
    _memory->write(0x02, 0x00);
    
    _registers->programCounter = 0x01;
    _registers->statusRegister = 0x00;
    _registers->accumulator = 0x08;
    
    adc(nes_emu::AddressMode::Immediate);
    
    EXPECT_EQ(_registers->programCounter, 0x02);
    EXPECT_EQ(_registers->accumulator, 0x87);
    EXPECT_EQ(_registers->statusRegister, nes_registers::StatusFlags::NegativeFlag | nes_registers::StatusFlags::OverflowFlag);
}

TEST_F(CPUTest, ADC_NegativeOverflow) {
    _memory->write(0x00, 0x69);
    _memory->write(0x01, 0x8F);
    _memory->write(0x02, 0x00);
    
    _registers->programCounter = 0x01;
    _registers->statusRegister = 0x00;
    _registers->accumulator = 0x80;
    
    adc(nes_emu::AddressMode::Immediate);
    
    EXPECT_EQ(_registers->programCounter, 0x02);
    EXPECT_EQ(_registers->accumulator, 0x0F);
    EXPECT_EQ(_registers->statusRegister, nes_registers::StatusFlags::CarryFlag | nes_registers::StatusFlags::OverflowFlag);
}

TEST_F(CPUTest, LDA_PositiveInteger) {
    _memory->write(0x00, 0xA9);
    _memory->write(0x01, 0x05);
    _memory->write(0x02, 0x00);
    
    _registers->programCounter = 0x01;
    _registers->statusRegister = 0x00;
    
    lda(nes_emu::AddressMode::Immediate);
    
    EXPECT_EQ(_registers->programCounter, 0x02);
    EXPECT_EQ(_registers->accumulator, 0x05);
    EXPECT_EQ(_registers->statusRegister, 0b00000000);
}

TEST_F(CPUTest, LDA_Zero) {
    _memory->write(0x00, 0xA9);
    _memory->write(0x01, 0x00);
    _memory->write(0x02, 0x00);
    
    _registers->programCounter = 0x01;
    _registers->statusRegister = 0x00;
    
    lda(nes_emu::AddressMode::Immediate);
    
    EXPECT_EQ(_registers->accumulator, 0x00);
    EXPECT_EQ(_registers->statusRegister, 0b00000010);
}

TEST_F(CPUTest, LDA_Immediate_NegativeInteger) {
    _memory->write(0x00, 0xA9);
    _memory->write(0x01, 0xF5);
    _memory->write(0x02, 0x00);
    
    _registers->programCounter = 0x01;
    _registers->statusRegister = 0x00;
    
    lda(nes_emu::AddressMode::Immediate);
    
    EXPECT_EQ(_registers->accumulator, 0xF5);
    EXPECT_EQ(_registers->statusRegister, 0b10000000);
}

TEST_F(CPUTest, LDA_Z_PositiveInteger) {
    _memory->write(0x00, 0xA5);
    _memory->write(0x01, 0xDB);
    _memory->write(0xDB, 0x05);

    _registers->programCounter = 0x01;
    _registers->statusRegister = 0x00;
    
    lda(nes_emu::AddressMode::ZeroPage);
    
    EXPECT_EQ(_registers->accumulator, 0x05);
    EXPECT_EQ(_registers->statusRegister, 0b00000000);
}

TEST_F(CPUTest, LDX_PositiveInteger) {
    _memory->write(0x00, 0xA2);
    _memory->write(0x01, 0x05);
    _memory->write(0x02, 0x00);
    
    _registers->programCounter = 0x01;
    _registers->statusRegister = 0x00;
    
    ldx(nes_emu::AddressMode::Immediate);
    
    EXPECT_EQ(_registers->programCounter, 0x02);
    EXPECT_EQ(_registers->x, 0x05);
    EXPECT_EQ(_registers->statusRegister, 0b00000000);
}

TEST_F(CPUTest, LDY_PositiveInteger) {
    _memory->write(0x00, 0xA0);
    _memory->write(0x01, 0x05);
    _memory->write(0x02, 0x00);
    
    _registers->programCounter = 0x01;
    _registers->statusRegister = 0x00;
    
    ldy(nes_emu::AddressMode::Immediate);
    
    EXPECT_EQ(_registers->programCounter, 0x02);
    EXPECT_EQ(_registers->y, 0x05);
    EXPECT_EQ(_registers->statusRegister, 0b00000000);
}

TEST_F(CPUTest, SBC_PositiveInteger) {
    _memory->write(0x00, 0xE9);
    _memory->write(0x01, 0x05);
    _memory->write(0x02, 0x00);
    
    _registers->programCounter = 0x01;
    _registers->statusRegister = 0x00;
    _registers->accumulator = 0x0B;
    
    sbc(nes_emu::AddressMode::Immediate);
    
    EXPECT_EQ(_registers->programCounter, 0x02);
    EXPECT_EQ(_registers->accumulator, 0x05);
    EXPECT_EQ(_registers->statusRegister, nes_registers::StatusFlags::CarryFlag);
}

TEST_F(CPUTest, SBC_NegativeInteger) {
    _memory->write(0x00, 0xE9);
    _memory->write(0x01, 0x0A);
    _memory->write(0x02, 0x00);
    
    _registers->programCounter = 0x01;
    _registers->statusRegister = 0x00;
    _registers->accumulator = 0x05;
    
    sbc(nes_emu::AddressMode::Immediate);
    
    EXPECT_EQ(_registers->programCounter, 0x02);
    EXPECT_EQ(_registers->accumulator, 0xFA);
    EXPECT_EQ(_registers->statusRegister, nes_registers::StatusFlags::NegativeFlag);
}

TEST_F(CPUTest, STA) {
    _memory->write(0x00, 0x85);
    _memory->write(0x01, 0xFC);
    _memory->write(0xFC, 0xDB);
    
    _registers->programCounter = 0x01;
    _registers->accumulator = 0xBD;
    
    sta(nes_emu::AddressMode::ZeroPage);
    
    EXPECT_EQ(_registers->programCounter, 0x02);
    EXPECT_EQ(_registers->accumulator, 0xBD);
    EXPECT_EQ(_memory->read_uint8(0xFC), 0xBD);
}

TEST_F(CPUTest, STX) {
    _memory->write(0x00, 0x85);
    _memory->write(0x01, 0xFC);
    _memory->write(0xFC, 0xDB);
    
    _registers->programCounter = 0x01;
    _registers->x = 0xBD;
    
    stx(nes_emu::AddressMode::ZeroPage);
    
    EXPECT_EQ(_registers->programCounter, 0x02);
    EXPECT_EQ(_registers->x, 0xBD);
    EXPECT_EQ(_memory->read_uint8(0xFC), 0xBD);
}

TEST_F(CPUTest, STY) {
    _memory->write(0x00, 0x85);
    _memory->write(0x01, 0xFC);
    _memory->write(0xFC, 0xDB);
    
    _registers->programCounter = 0x01;
    _registers->y = 0xBD;
    
    sty(nes_emu::AddressMode::ZeroPage);
    
    EXPECT_EQ(_registers->programCounter, 0x02);
    EXPECT_EQ(_registers->y, 0xBD);
    EXPECT_EQ(_memory->read_uint8(0xFC), 0xBD);
}

TEST_F(CPUTest, TAX_PositiveInteger) {
    _registers->accumulator = 0x05;
    _registers->x = 0xDB;
    _registers->statusRegister = 0x00;
    
    tax();
    
    EXPECT_EQ(_registers->accumulator, 0x05);
    EXPECT_EQ(_registers->x, 0x05);
    EXPECT_EQ(_registers->statusRegister, 0b00000000);
}
}
