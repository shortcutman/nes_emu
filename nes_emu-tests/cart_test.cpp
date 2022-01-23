//
//  cart_test.cpp
//  nes_emu-tests
//
//  Created by Daniel Burke on 21/1/2022.
//

#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

#include "cartridge.hpp"

namespace {

TEST(CartridgeTest, readCart) {
    std::shared_ptr<nes_emu::Cartridge> cart;
        
    std::ifstream cartFile("/Users/daniel/Projects.nosync/nes_emu/nes_emu/nes_emu-tests/nestest/nestest.nes", std::fstream::in | std::fstream::binary);
    
    ASSERT_TRUE(cartFile.good());
    
    cart = nes_emu::Cartridge::cartridgeFromStream(cartFile);
    
    EXPECT_EQ(cart->mirrorType(), nes_emu::Cartridge::MirrorType::Horizontal);
    
    EXPECT_EQ(cart->readCart(0x8000), 0x4c);
    EXPECT_EQ(cart->readCart(0x8001), 0xf5);
    EXPECT_EQ(cart->readCart(0x8002), 0xc5);
    EXPECT_EQ(cart->readCart(0x8003), 0x60);
    
    EXPECT_EQ(cart->readCart(0xc000), 0x4c);
    EXPECT_EQ(cart->readCart(0xc001), 0xf5);
    EXPECT_EQ(cart->readCart(0xc002), 0xc5);
    EXPECT_EQ(cart->readCart(0xc003), 0x60);
}

}
