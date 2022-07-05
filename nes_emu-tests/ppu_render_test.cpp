//
//  ppu_render_test.cpp
//  nes_emu-tests
//
//  Created by Daniel Burke on 5/7/2022.
//

#include <gtest/gtest.h>

#include "ppu.hpp"

namespace {

class PPURenderTest : public nes_emu::PPU, public ::testing::Test {
};

TEST_F(PPURenderTest, ConstructTile) {
    std::array<uint8_t, 16> tile = {{
        0xE0, 0xC0, 0x80, 0xFC, 0x80, 0xC0, 0x00, 0x20,
        0x00, 0x20, 0x60, 0x00, 0xF0, 0xFC, 0xFE, 0xFE
    }};
    
    std::array<uint8_t, 64> tilePaletteIndexes = {{
        1, 1, 1, 0, 0, 0, 0, 0,
        1, 1, 2, 0, 0, 0, 0, 0,
        1, 2, 2, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 0, 0,
        3, 2, 2, 2, 0, 0, 0, 0,
        3, 3, 2, 2, 2, 2, 0, 0,
        2, 2, 2, 2, 2, 2, 2, 0,
        2, 2, 3, 2, 2, 2, 2, 0
    }};
    
    auto result = constructTile(&tile[0]);
    
    EXPECT_EQ(result, tilePaletteIndexes);
}

}
