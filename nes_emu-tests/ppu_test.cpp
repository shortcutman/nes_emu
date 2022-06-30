//
//  ppu_test.cpp
//  nes_emu-tests
//
//  Created by Daniel Burke on 30/6/2022.
//

#include <gtest/gtest.h>

#include "ppu.hpp"

namespace {

class PPUTest : public nes_emu::PPU, public ::testing::Test {
};

TEST_F(PPUTest, ReadFromVRAM) {
    _vram[0xDB] = 0xDB;
    
    writeRegister_uint8(0x2006, 0x20);
    writeRegister_uint8(0x2006, 0xDB);
    
    //Per the rust tutorial:
    //Because CHR ROM and RAM are considered external devices to PPU, PPU can't return
    //the value immediately. PPU has to fetch the data and keep it in internal buffer.
    //The first read from 0x2007 would return the content of this internal buffer filled
    //during the previous load operation. From the CPU perspective, this is a dummy read.
    auto dummyRead = readRegister_uint8(0x2007);
    EXPECT_EQ(dummyRead, 0);
    
    auto actualRead = readRegister_uint8(0x2007);
    EXPECT_EQ(actualRead, 0xDB);
}

}
