//
//  util_test.cpp
//  nes_emu-tests
//
//  Created by Daniel Burke on 15/1/2022.
//

#include <gtest/gtest.h>

#include "instruction_utils.hpp"

#include "registers.hpp"
#include "memory.hpp"

namespace {

TEST(Instruction_Utils, decodeOperandAddress_Immediate) {
    nes_registers r;
    r.programCounter = 0x01;
    nes_memory m;
    m.write(0x01, 0xDB);
    
    auto result = decodeOperandAddress(nes_emu::AddressMode::Immediate, r, m);
    
    EXPECT_EQ(result, 0x01);
    EXPECT_EQ(r.programCounter, 0x02);
}

TEST(Instruction_Utils, decodeOperandAddress_Absolute) {
    nes_registers r;
    r.programCounter = 0x01;
    nes_memory m;
    m.write(0x01, 0xDB);
    m.write(0x02, 0xFE);
    
    auto result = decodeOperandAddress(nes_emu::AddressMode::Absolute, r, m);
    
    EXPECT_EQ(result, 0xFEDB);
    EXPECT_EQ(r.programCounter, 0x03);
}

TEST(Instruction_Utils, decodeOperandAddress_ZeroPage) {
    nes_registers r;
    r.programCounter = 0x01;
    nes_memory m;
    m.write(0x00, 0x00);
    m.write(0x01, 0xDB);
    m.write(0x02, 0xFE);
    m.write(0xDB, 0xCC);
    
    auto result = decodeOperandAddress(nes_emu::AddressMode::ZeroPage, r, m);
    
    EXPECT_EQ(result, 0xDB);
    EXPECT_EQ(r.programCounter, 0x02);
}

TEST(Instruction_Utils, statusFlags_PositiveInt) {
    auto result = nes_emu::statusFlagsOnByteValue(0x05, 0x00);
    EXPECT_EQ(result, 0b00000000);
}

TEST(Instruction_Utils, statusFlags_Zero) {
    auto result = nes_emu::statusFlagsOnByteValue(0x00, 0x00);
    EXPECT_EQ(result, 0b00000010);
}

TEST(Instruction_Utils, statusFlags_NegativeInt) {
    auto result = nes_emu::statusFlagsOnByteValue(0xF5, 0x00);
    EXPECT_EQ(result, 0b10000000);
}

}
