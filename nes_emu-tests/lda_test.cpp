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
TEST(Instructions, LDA_PositiveInteger) {
    nes_memory m;
    m.write(0x00, 0xA9);
    m.write(0x01, 0x05);
    m.write(0x02, 0x00);
    
    nes_registers r;
    r.programCounter = 0x01;
    r.statusRegister = 0x00;
    
    nes_emu::lda_immediate(r, m);
    
    EXPECT_EQ(r.programCounter, 0x02);
    EXPECT_EQ(r.accumulator, 0x05);
    EXPECT_EQ(r.statusRegister, 0b00000000);
}

TEST(Instructions, LDA_Zero) {
    nes_memory m;
    m.write(0x00, 0xA9);
    m.write(0x01, 0x00);
    m.write(0x02, 0x00);
    
    nes_registers r;
    r.programCounter = 0x01;
    r.statusRegister = 0x00;
    
    nes_emu::lda_immediate(r, m);
    
    EXPECT_EQ(r.accumulator, 0x00);
    EXPECT_EQ(r.statusRegister, 0b00000010);
}

TEST(Instructions, LDA_Immediate_NegativeInteger) {
    nes_memory m;
    m.write(0x00, 0xA9);
    m.write(0x01, 0xF5);
    m.write(0x02, 0x00);
    
    nes_registers r;
    r.programCounter = 0x01;
    r.statusRegister = 0x00;
    
    nes_emu::lda_immediate(r, m);
    
    EXPECT_EQ(r.accumulator, 0xF5);
    EXPECT_EQ(r.statusRegister, 0b10000000);
}

TEST(Instructions, LDA_Z_PositiveInteger) {
    nes_memory m;
    m.write(0x00, 0xA5);
    m.write(0x01, 0xDB);
    m.write(0xDB, 0x05);

    nes_registers r;
    r.programCounter = 0x01;
    r.statusRegister = 0x00;
    
    nes_emu::lda_zero(r, m);
    
    EXPECT_EQ(r.accumulator, 0x05);
    EXPECT_EQ(r.statusRegister, 0b00000000);
}


TEST(Instructions, TAX_PositiveInteger) {
    nes_registers r;
    r.accumulator = 0x05;
    r.x = 0xDB;
    r.statusRegister = 0x00;
    
    nes_emu::tax(r);
    
    EXPECT_EQ(r.accumulator, 0x05);
    EXPECT_EQ(r.x, 0x05);
    EXPECT_EQ(r.statusRegister, 0b00000000);
}
}
