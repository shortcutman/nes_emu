//
//  ppu_render.cpp
//  instructions
//
//  Created by Daniel Burke on 2/7/2022.
//

#include "ppu.hpp"

#include "cartridge.hpp"

#include <cstdint>
#include <tuple>
#include <array>

namespace {

typedef std::tuple<uint8_t, uint8_t, uint8_t, uint8_t> Colour;
typedef std::array<Colour, 64> Palette;

const Palette SystemPalette = {{
    {0x80, 0x80, 0x80, 0xFF}, {0x00, 0x3D, 0xA6, 0xFF}, {0x00, 0x12, 0xB0, 0xFF}, {0x44, 0x00, 0x96, 0xFF}, {0xA1, 0x00, 0x5E, 0xFF},
       {0xC7, 0x00, 0x28, 0xFF}, {0xBA, 0x06, 0x00, 0xFF}, {0x8C, 0x17, 0x00, 0xFF}, {0x5C, 0x2F, 0x00, 0xFF}, {0x10, 0x45, 0x00, 0xFF},
       {0x05, 0x4A, 0x00, 0xFF}, {0x00, 0x47, 0x2E, 0xFF}, {0x00, 0x41, 0x66, 0xFF}, {0x00, 0x00, 0x00, 0xFF}, {0x05, 0x05, 0x05, 0xFF},
       {0x05, 0x05, 0x05, 0xFF}, {0xC7, 0xC7, 0xC7, 0xFF}, {0x00, 0x77, 0xFF, 0xFF}, {0x21, 0x55, 0xFF, 0xFF}, {0x82, 0x37, 0xFA, 0xFF},
       {0xEB, 0x2F, 0xB5, 0xFF}, {0xFF, 0x29, 0x50, 0xFF}, {0xFF, 0x22, 0x00, 0xFF}, {0xD6, 0x32, 0x00, 0xFF}, {0xC4, 0x62, 0x00, 0xFF},
       {0x35, 0x80, 0x00, 0xFF}, {0x05, 0x8F, 0x00, 0xFF}, {0x00, 0x8A, 0x55, 0xFF}, {0x00, 0x99, 0xCC, 0xFF}, {0x21, 0x21, 0x21, 0xFF},
       {0x09, 0x09, 0x09, 0xFF}, {0x09, 0x09, 0x09, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF}, {0x0F, 0xD7, 0xFF, 0xFF}, {0x69, 0xA2, 0xFF, 0xFF},
       {0xD4, 0x80, 0xFF, 0xFF}, {0xFF, 0x45, 0xF3, 0xFF}, {0xFF, 0x61, 0x8B, 0xFF}, {0xFF, 0x88, 0x33, 0xFF}, {0xFF, 0x9C, 0x12, 0xFF},
       {0xFA, 0xBC, 0x20, 0xFF}, {0x9F, 0xE3, 0x0E, 0xFF}, {0x2B, 0xF0, 0x35, 0xFF}, {0x0C, 0xF0, 0xA4, 0xFF}, {0x05, 0xFB, 0xFF, 0xFF},
       {0x5E, 0x5E, 0x5E, 0xFF}, {0x0D, 0x0D, 0x0D, 0xFF}, {0x0D, 0x0D, 0x0D, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF}, {0xA6, 0xFC, 0xFF, 0xFF},
       {0xB3, 0xEC, 0xFF, 0xFF}, {0xDA, 0xAB, 0xEB, 0xFF}, {0xFF, 0xA8, 0xF9, 0xFF}, {0xFF, 0xAB, 0xB3, 0xFF}, {0xFF, 0xD2, 0xB0, 0xFF},
       {0xFF, 0xEF, 0xA6, 0xFF}, {0xFF, 0xF7, 0x9C, 0xFF}, {0xD7, 0xE8, 0x95, 0xFF}, {0xA6, 0xED, 0xAF, 0xFF}, {0xA2, 0xF2, 0xDA, 0xFF},
    {0x99, 0xFF, 0xFC, 0xFF}, {0xDD, 0xDD, 0xDD, 0xFF}, {0x11, 0x11, 0x11, 0xFF}, {0x11, 0x11, 0x11, 0xFF}
}};

const uint16_t FrameWidth = 256;
const uint16_t FrameHeight = 240;

const uint16_t TileSizeBytes = 16;
const uint16_t TileWidthPixels = 8;
const uint16_t TileHeightPixels = 8;
const uint16_t BackgroundTileCount = FrameWidth / TileWidthPixels * FrameHeight / TileHeightPixels;

const uint16_t PatternTableSizeBytes = 0x1000;
const uint16_t NameTableSizeBytes = 0x400;
}

nes_emu::PPU::Frame nes_emu::PPU::renderFrame() {
    PPU::Frame frame;
    
    renderBackgroundTiles(frame);
    
    return frame;
}

nes_emu::PPU::Frame nes_emu::PPU::renderPatternTableToFrame() {
    PPU::Frame frame;
    
    for (uint16_t t = 0; t < 256; t++) {
        uint16_t xOffset = t % 20 * 8;
        uint16_t yOffset = t / 20 * 8;

        auto tile = constructTile(_cartridge->readCHRRomDirect(0 * 0x1000 + t * 0x10));
        auto colouredTile = colourTile(0, xOffset/8, yOffset/8, tile);

        for (uint8_t x = 0; x < 8; x++) {
            for (uint8_t y = 0; y < 8; y++) {
                frame[x + xOffset + (y + yOffset) * 256] = colouredTile[x + y * 8];
            }
        }
    }
    
    return frame;
}

void nes_emu::PPU::renderBackgroundTiles(Frame &frame) {
    for (uint16_t bgIndex = 0; bgIndex < BackgroundTileCount; bgIndex++) {
        uint16_t xOffset = bgIndex % 32;
        uint16_t yOffset = bgIndex / 32;

        auto tileIndex = _vram[bgIndex];
        uint8_t bank = (_controlRegister & 0x10) ? 1 : 0;
        uint16_t tileByte = bank * PatternTableSizeBytes + tileIndex * TileSizeBytes;
        auto tile = constructTile(_cartridge->readCHRRomDirect(tileByte));
        auto colouredTile = colourTile(bank, xOffset, yOffset, tile);

        for (uint8_t x = 0; x < 8; x++) {
            for (uint8_t y = 0; y < 8; y++) {
                frame[x + xOffset * 8 + (y + (yOffset*8)) * FrameWidth] = colouredTile[x + y * 8];
            }
        }
    }
}

std::array<uint8_t, 64> nes_emu::PPU::constructTile(const uint8_t* data) {
    if (data == nullptr) {
        throw std::logic_error("require pointer to data");
    }
        
    std::array<uint8_t, 64> tile;
    
    for (uint8_t y = 0; y < 8; y++) {
        auto loByte = *(data + y);
        auto hiByte = *(data + y + 8);
        
        for (uint8_t x = 0; x < 8; x++) {
            uint8_t hiBit = ((( hiByte >> (7-x) ) & 0x1) << 1);
            uint8_t loBit = ((loByte >> (7-x)) & 0x1);
            tile[y * 8 + x] = hiBit | loBit;
        }
    }
    
    return tile;
}

std::array<nes_emu::PPU::Colour, 64> nes_emu::PPU::colourTile(
                                                              uint8_t nametable,
                                                              uint8_t tileX,
                                                              uint8_t tileY,
                                                              std::array<uint8_t, 64>& tile) {
    std::array<nes_emu::PPU::Colour, 64> colouredTile;
    uint8_t attributeTableIndex = tileX / 4 + (tileY / 4) * 8;
    uint8_t attributeTableValue = _vram[nametable * NameTableSizeBytes + BackgroundTileCount + attributeTableIndex];
    
    uint8_t metatileX = tileX % 4 / 2;
    uint8_t metatileY = tileY % 4 / 2;
    uint8_t bits = (metatileY * 2 + metatileX) * 2;
    uint8_t paletteStartByte = ((attributeTableValue >> bits) & 0b11) * 0x04;
    
    std::array<PPU::Colour, 4> palette;
    palette[0] = SystemPalette[_paletteRAM[0x00]];
    palette[1] = SystemPalette[_paletteRAM[1 + paletteStartByte + 0]];
    palette[2] = SystemPalette[_paletteRAM[1 + paletteStartByte + 1]];
    palette[3] = SystemPalette[_paletteRAM[1 + paletteStartByte + 2]];
    
    for (uint8_t x = 0; x < 8; x++) {
        for (uint8_t y = 0; y < 8; y++) {
            uint8_t index = x + y * 8;
            colouredTile[index] = palette[tile[index]];
        }
    }
    
    return colouredTile;
}
