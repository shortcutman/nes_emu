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

TEST_F(APUTest, WritePulse1) {
    this->writeRegister(0x4000, 0b01110101);
    EXPECT_EQ(this->_pulse1._duty, 1);
    EXPECT_EQ(this->_pulse1._lengthCounterHalt, 1);
    EXPECT_EQ(this->_pulse1._constantVolume, 1);
    EXPECT_EQ(this->_pulse1._volume, 5);

    this->writeRegister(0x4001, 0b01110101);
    EXPECT_EQ(this->_pulse1._sweepEnabled, 0);
    EXPECT_EQ(this->_pulse1._sweepPeriod, 7);
    EXPECT_EQ(this->_pulse1._sweepNegate, 0);
    EXPECT_EQ(this->_pulse1._sweepShift, 5);

    this->writeRegister(0x4002, 0x44);
    EXPECT_EQ(this->_pulse1._timer, 0x44);

    this->writeRegister(0x4003, 0x44);
    EXPECT_EQ(this->_pulse1._lengthCounterLoad, 0x8);
    EXPECT_EQ(this->_pulse1._timer, 0x444);
}

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

TEST_F(APUTest, WriteFrameCounter) {
    this->writeRegister(0x4017, 0);
    EXPECT_EQ(this->_mode, 0);

    this->writeRegister(0x4017, 0x80);
    EXPECT_EQ(this->_mode, 1);

    this->writeRegister(0x4017, 0);
    EXPECT_EQ(this->_mode, 0);
}

}