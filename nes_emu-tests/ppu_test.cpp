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

const uint64_t PPUPowerUpCycles = 29658 * 3;

class PPUTest : public nes_emu::PPU, public ::testing::Test {
};

TEST_F(PPUTest, ControlNoopAtPowerOn) {
    
    writeControlRegister(0xCD);
    EXPECT_EQ(_controlRegister, 0x00);
    
    this->_ppuCycles = PPUPowerUpCycles + 1;
    
    writeControlRegister(0xCD);
    EXPECT_EQ(_controlRegister, 0xCD);
}

TEST_F(PPUTest, BasicReadFromVRAM) {
    auto cart = nes_emu::Cartridge::emptyCartridge(nes_emu::Cartridge::MirrorType::Horizontal);
    this->_cartridge = cart;
    this->_ppuCycles = PPUPowerUpCycles + 1;
    
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

TEST_F(PPUTest, ReadFromVRAMx3Mirror) {
    auto cart = nes_emu::Cartridge::emptyCartridge(nes_emu::Cartridge::MirrorType::Horizontal);
    this->_cartridge = cart;
    this->_ppuCycles = PPUPowerUpCycles + 1;
    
    for (uint32_t i = 0; i < _vram.size(); i++) {
        _vram[i] = i;
    }
    
    writeRegister_uint8(0x2006, 0x30);
    writeRegister_uint8(0x2006, 0xDB);
    
    auto dummyRead = readRegister_uint8(0x2007);
    EXPECT_EQ(dummyRead, 0);
    
    auto actualRead = readRegister_uint8(0x2007);
    EXPECT_EQ(actualRead, 0xDB);
}

TEST_F(PPUTest, ReadFromVRAMIncrement32) {
    auto cart = nes_emu::Cartridge::emptyCartridge(nes_emu::Cartridge::MirrorType::Horizontal);
    this->_cartridge = cart;
    this->_ppuCycles = PPUPowerUpCycles + 1;
    
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
    this->_ppuCycles = PPUPowerUpCycles + 1;
    
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
    this->_ppuCycles = PPUPowerUpCycles + 1;
    
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
    this->_ppuCycles = PPUPowerUpCycles + 1;
    
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

TEST_F(PPUTest, OAMReadWrite) {
    writeOAMAddressRegister(0xF0);
    writeOAMDataRegister(0xAA);
    writeOAMDataRegister(0xBB);
    writeOAMDataRegister(0xCC);
    
    writeOAMAddressRegister(0xF0);
    EXPECT_EQ(readOAMDataRegister(), 0xAA);
    writeOAMAddressRegister(0xF1);
    EXPECT_EQ(readOAMDataRegister(), 0xBB);
    writeOAMAddressRegister(0xF2);
    EXPECT_EQ(readOAMDataRegister(), 0xCC);
}

TEST_F(PPUTest, OAMDMA) {
    std::array<uint8_t, 256> testPage;
    for (uint16_t i = 0; i < 256; i++) {
        testPage[i] = i;
    }
    
    oamDMA(testPage);
    
    for (uint16_t i = 0; i < 256; i++) {
        writeOAMAddressRegister(i);
        EXPECT_EQ(readOAMDataRegister(), i);
    }
}

TEST_F(PPUTest, ClockScanLineSingleIncrement) {
    bool nmiInterruptTest = false;
    
    EXPECT_EQ(readStatusRegister(), 0x00);
    
    advanceClockAndCheckInterrupt(341, nmiInterruptTest);
    
    EXPECT_EQ(_scanLine, 1);
    EXPECT_EQ(nmiInterruptTest, false);
    EXPECT_EQ(readStatusRegister(), 0x00);
}

TEST_F(PPUTest, ClockScanLineMultipleIncrementEnableInterrupt) {
    bool nmiInterruptTest = false;
    _controlRegister = 0x80;
    
    EXPECT_EQ(_scanLine, 0);
    EXPECT_EQ(readStatusRegister(), 0x00);
    
    for (int i = 1; i < 262; i++) {
        advanceClockAndCheckInterrupt(341, nmiInterruptTest);
        EXPECT_EQ(_scanLine, i);
        EXPECT_EQ(nmiInterruptTest, i == 241);
        EXPECT_EQ(readStatusRegister(), i == 241 ? 0x80 : 0x00);
    }
    
    advanceClockAndCheckInterrupt(341, nmiInterruptTest);
    EXPECT_EQ(_scanLine, 0);
    EXPECT_EQ(nmiInterruptTest, false);
    EXPECT_EQ(readStatusRegister(), 0x00);
}

TEST_F(PPUTest, ClockScanLineMultipleIncrementDisableInterrupt) {
    bool nmiInterruptTest = false;
    _controlRegister = 0x00;
    
    EXPECT_EQ(_scanLine, 0);
    EXPECT_EQ(readStatusRegister(), 0x00);
    
    for (int i = 1; i < 262; i++) {
        advanceClockAndCheckInterrupt(341, nmiInterruptTest);
        EXPECT_EQ(_scanLine, i);
        EXPECT_EQ(nmiInterruptTest, false);
        EXPECT_EQ(readStatusRegister(), i == 241 ? 0x80 : 0x00);
    }
    
    advanceClockAndCheckInterrupt(341, nmiInterruptTest);
    EXPECT_EQ(_scanLine, 0);
    EXPECT_EQ(nmiInterruptTest, false);
    EXPECT_EQ(readStatusRegister(), 0x00);
}

TEST_F(PPUTest, ClockScanLineVSyncFlagClearInterruptEnable) {
    bool nmiInterruptTest = false;
    _controlRegister = 0x80;
    
    _scanLine = 240;
    EXPECT_EQ(readStatusRegister(), 0x00);
    
    advanceClockAndCheckInterrupt(341, nmiInterruptTest);
    EXPECT_EQ(_scanLine, 241);
    EXPECT_EQ(nmiInterruptTest, true);
    EXPECT_EQ(readStatusRegister(), 0x80);
    EXPECT_EQ(readStatusRegister(), 0x00); //flag should be cleared after read
    
    advanceClockAndCheckInterrupt(341, nmiInterruptTest);
    EXPECT_EQ(_scanLine, 242);
    EXPECT_EQ(nmiInterruptTest, false);
    EXPECT_EQ(readStatusRegister(), 0x00);
}

TEST_F(PPUTest, ClockScanLineVSyncFlagClearInterruptDisable) {
    bool nmiInterruptTest = false;
    _controlRegister = 0x00;
    
    _scanLine = 240;
    EXPECT_EQ(readStatusRegister(), 0x00);
    
    advanceClockAndCheckInterrupt(341, nmiInterruptTest);
    EXPECT_EQ(_scanLine, 241);
    EXPECT_EQ(nmiInterruptTest, false);
    EXPECT_EQ(readStatusRegister(), 0x80);
    EXPECT_EQ(readStatusRegister(), 0x00); //flag should be cleared after read
    
    advanceClockAndCheckInterrupt(341, nmiInterruptTest);
    EXPECT_EQ(_scanLine, 242);
    EXPECT_EQ(nmiInterruptTest, false);
    EXPECT_EQ(readStatusRegister(), 0x00);
}


TEST_F(PPUTest, ClockScanLineNMIInterrupt) {
    EXPECT_THROW({
        bool nmiInterruptTest = false;
        advanceClockAndCheckInterrupt(89344, nmiInterruptTest);
    }, std::runtime_error);
}

}
