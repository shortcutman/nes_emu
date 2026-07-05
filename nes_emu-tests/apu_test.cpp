//
//  apu_test.cpp
//  nes_emu-tests
//
//  Created by Daniel Burke on 05/7/2026.
//

#include <gtest/gtest.h>

#include "apu.hpp"

namespace {
class APUTest : public nes_emu::APU, public ::testing::Test {
};

TEST_F(APUTest, WriteStatus) {
    this->writeRegister(0x4015, 0);
    EXPECT_EQ(this->_enableDMC, false);
    EXPECT_EQ(this->_enableNoise, false);
    EXPECT_EQ(this->_enableTriangle, false);
    EXPECT_EQ(this->_enablePulse1, false);
    EXPECT_EQ(this->_enablePulse2, false);

    this->writeRegister(0x4015, 0x1f);
    EXPECT_EQ(this->_enableDMC, true);
    EXPECT_EQ(this->_enableNoise, true);
    EXPECT_EQ(this->_enableTriangle, true);
    EXPECT_EQ(this->_enablePulse1, true);
    EXPECT_EQ(this->_enablePulse2, true);

    this->writeRegister(0x4015, 0);
    EXPECT_EQ(this->_enableDMC, false);
    EXPECT_EQ(this->_enableNoise, false);
    EXPECT_EQ(this->_enableTriangle, false);
    EXPECT_EQ(this->_enablePulse1, false);
    EXPECT_EQ(this->_enablePulse2, false);
}

}