//
//  integration_test.cpp
//  nes_emu-tests
//
//  Created by Daniel Burke on 22/1/2022.
//

#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

#include "cartridge.hpp"
#include "memory.hpp"
#include "cpu.hpp"
#include "registers.hpp"

namespace {

using namespace nes_emu;

class IntegrationTest : public nes_emu::CPU, public ::testing::Test {
};

TEST_F(IntegrationTest, nestest) {
    std::ifstream cartFile("/Users/daniel/Projects.nosync/nes_emu/nes_emu/nes_emu-tests/nestest/nestest.nes", std::fstream::in | std::fstream::binary);

    ASSERT_TRUE(cartFile.good());

    _memory->_cartridge = nes_emu::Cartridge::cartridgeFromStream(cartFile);

    _registers->programCounter = 0x8000;

    uint64_t count = 0;
    
    try {
    while (1) {
        executeOne();
        count++;
    }
    } catch (std::exception& e) {
        
        std::cout << "Exception after " << count << " instructions: " << e.what() << std::endl;
    }
}

}
