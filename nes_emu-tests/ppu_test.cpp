//
//  ppu_test.cpp
//  nes_emu-tests
//
//  Created by Daniel Burke on 30/6/2022.
//

#include <gtest/gtest.h>

#include "ppu.hpp"
#include "cartridge.hpp"

namespace {

class PPUTest : public nes_emu::PPU, public ::testing::Test {
};

TEST_F(PPUTest, BasicReadFromVRAM) {
    auto cart = nes_emu::Cartridge::emptyCartridge(nes_emu::Cartridge::MirrorType::Horizontal);
    this->_cartridge = cart;
    
    for (uint32_t i = 0; i < _vram.size(); i++) {
        _vram[i] = i;
    }
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

TEST_F(PPUTest, ReadFromVRAMIncrement32) {
    auto cart = nes_emu::Cartridge::emptyCartridge(nes_emu::Cartridge::MirrorType::Horizontal);
    this->_cartridge = cart;
    
    writeControlRegister(0b00000010);
    
    for (uint32_t i = 0; i < _vram.size(); i++) {
        _vram[i] = i;
    }
    
    writeRegister_uint8(0x2006, 0x20);
    writeRegister_uint8(0x2006, 0xDB);
    
    auto dummyRead = readRegister_uint8(0x2007);
    EXPECT_EQ(dummyRead, 0);
    
    auto actualRead = readRegister_uint8(0x2007);
    EXPECT_EQ(actualRead, 0xDB);
    
    auto incrementRead = readRegister_uint8(0x2007);
    EXPECT_EQ(incrementRead, 0xFB);
}

TEST_F(PPUTest, ReadFromVRAMIncrement1) {
    auto cart = nes_emu::Cartridge::emptyCartridge(nes_emu::Cartridge::MirrorType::Horizontal);
    this->_cartridge = cart;
    
    writeControlRegister(0b00000000);
    
    for (uint32_t i = 0; i < _vram.size(); i++) {
        _vram[i] = i;
    }
    
    writeRegister_uint8(0x2006, 0x20);
    writeRegister_uint8(0x2006, 0xDB);
    
    auto dummyRead = readRegister_uint8(0x2007);
    EXPECT_EQ(dummyRead, 0);
    
    auto actualRead = readRegister_uint8(0x2007);
    EXPECT_EQ(actualRead, 0xDB);
    
    auto incrementRead = readRegister_uint8(0x2007);
    EXPECT_EQ(incrementRead, 0xDC);
}


TEST_F(PPUTest, VRAMHorizontalMirror) {
    auto cart = nes_emu::Cartridge::emptyCartridge(nes_emu::Cartridge::MirrorType::Horizontal);
    this->_cartridge = cart;
    
    _vram[0x00DB] = 0xDB;
    _vram[0x04DB] = 0xBD;
    
    writeRegister_uint8(0x2006, 0x20);
    writeRegister_uint8(0x2006, 0xDB);
    EXPECT_EQ(readRegister_uint8(0x2007), 0);
    EXPECT_EQ(readRegister_uint8(0x2007), 0xDB);
    
    writeRegister_uint8(0x2006, 0x24);
    writeRegister_uint8(0x2006, 0xDB);
    readRegister_uint8(0x2007);
    EXPECT_EQ(readRegister_uint8(0x2007), 0xDB);
    
    writeRegister_uint8(0x2006, 0x28);
    writeRegister_uint8(0x2006, 0xDB);
    readRegister_uint8(0x2007);
    EXPECT_EQ(readRegister_uint8(0x2007), 0xBD);
    
    writeRegister_uint8(0x2006, 0x2C);
    writeRegister_uint8(0x2006, 0xDB);
    readRegister_uint8(0x2007);
    EXPECT_EQ(readRegister_uint8(0x2007), 0xBD);
}

TEST_F(PPUTest, VRAMVerticalMirror) {
    auto cart = nes_emu::Cartridge::emptyCartridge(nes_emu::Cartridge::MirrorType::Vertical);
    this->_cartridge = cart;
    
    _vram[0x00DB] = 0xDB;
    _vram[0x04DB] = 0xBD;
    
    writeRegister_uint8(0x2006, 0x20);
    writeRegister_uint8(0x2006, 0xDB);
    EXPECT_EQ(readRegister_uint8(0x2007), 0);
    EXPECT_EQ(readRegister_uint8(0x2007), 0xDB);
    
    writeRegister_uint8(0x2006, 0x24);
    writeRegister_uint8(0x2006, 0xDB);
    readRegister_uint8(0x2007);
    EXPECT_EQ(readRegister_uint8(0x2007), 0xBD);
    
    writeRegister_uint8(0x2006, 0x28);
    writeRegister_uint8(0x2006, 0xDB);
    readRegister_uint8(0x2007);
    EXPECT_EQ(readRegister_uint8(0x2007), 0xDB);
    
    writeRegister_uint8(0x2006, 0x2C);
    writeRegister_uint8(0x2006, 0xDB);
    readRegister_uint8(0x2007);
    EXPECT_EQ(readRegister_uint8(0x2007), 0xBD);
}

}
