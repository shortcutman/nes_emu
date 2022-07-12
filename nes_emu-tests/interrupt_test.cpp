//
//  interrupt_test.cpp
//  nes_emu-tests
//
//  Created by Daniel Burke on 6/2/2022.
//

#include <gtest/gtest.h>

#include "cpu.hpp"
#include "registers.hpp"
#include "memory.hpp"
#include "cartridge.hpp"

namespace test {
class InterruptTest : public nes_emu::CPU, public ::testing::Test {
    void SetUp() override {
        this->setMemory(std::make_shared<nes_emu::Memory>());
    }
};

TEST_F(InterruptTest, Reset) {
    auto cart = std::make_shared<nes_emu::Cartridge>();
    cart->_prgROM.resize(16384, 0);
    cart->_prgROM[16380] = 0x01;
    cart->_prgROM[16381] = 0x02;
    
    _memory->setCartridge(cart);
    _memory->write(0x0201, 0xEA);
    _registers->setStatusFlag(nes_registers::StatusFlags::InterruptDisable, false);
    _interrupt = nes_emu::CPU::Interrupt::Reset;
    executeOne();
    
    EXPECT_EQ(_registers->programCounter, 0x0201);
    EXPECT_TRUE(_registers->getStatusFlag(nes_registers::StatusFlags::InterruptDisable));
}

TEST_F(InterruptTest, Reset_Disable) {
    auto cart = std::make_shared<nes_emu::Cartridge>();
    cart->_prgROM.resize(16384, 0);
    cart->_prgROM[16380] = 0x01;
    cart->_prgROM[16381] = 0x02;
    
    _memory->setCartridge(cart);
    _memory->write(0x00, 0xEA);
    _memory->write(0x01, 0xEA);
    _memory->write(0x0201, 0xEA);
    
    _registers->programCounter = 0x00;
    _registers->setStatusFlag(nes_registers::StatusFlags::InterruptDisable, true);
    _interrupt = nes_emu::CPU::Interrupt::Reset;
    executeOne();
    
    EXPECT_EQ(_registers->programCounter, 0x01);
    EXPECT_TRUE(_registers->getStatusFlag(nes_registers::StatusFlags::InterruptDisable));
}
}
