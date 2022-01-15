//
//  util_test.cpp
//  nes_emu-tests
//
//  Created by Daniel Burke on 15/1/2022.
//

#include <gtest/gtest.h>

#include "instruction_utils.hpp"

namespace {

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
