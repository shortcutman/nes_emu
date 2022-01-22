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
}

}
