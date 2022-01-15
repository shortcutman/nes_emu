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
