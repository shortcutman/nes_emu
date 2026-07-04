//
//  ppu.hpp
//  instructions
//
//  Created by Daniel Burke on 4/2/2022.
//

#ifndef ppu_hpp
#define ppu_hpp

#include <cstdint>
#include <memory>
#include <array>

namespace nes_emu {
    class Cartridge;

    class PPU {
    public:
        typedef std::tuple<uint8_t, uint8_t, uint8_t, uint8_t> Colour;
        typedef std::array<Colour, 8*8> ColouredTile;

        struct Rect {
            int t, b;
            int l, r;
        };

        struct Shift {
            int x, y;
        };
        
    protected:
        enum PPUControl : uint8_t {
            NameTableAddressBits = 0x03,
            VRAMAddressIncrement = 0x04,
            SpritePatternTableAddress = 0x08,
            BackgroundPatternTableAddress = 0x10,
            SpriteSize = 0x20,
            PPUMasterSlaveSelect = 0x40,
            GenerateNMI = 0x80,
        };

        enum PPUMask : uint8_t {
            Greyscale = 0x01,
            ShowBGLeftMostEight = 0x02,
            ShowSpriteLeftMostEight = 0x04,
            EnableBackground = 0x08,
            EnableSprite = 0x10,
            EmphasizeRed = 0x20,
            EmphasizeGreen = 0x40,
            EmphasizeBlue = 0x80,
        };
        
        enum PPUStatus : uint8_t {
            SpriteOverflow = 0x20,
            SpriteZeroHit = 0x40,
            VerticalBlankStarted = 0x80
        };
        
    protected:
        std::shared_ptr<Cartridge> _cartridge;
        std::array<uint8_t, 2048>  _vram;
        std::array<uint8_t, 32>  _paletteRAM;
        std::array<uint8_t, 256>  _oam;
        
        uint8_t _controlRegister;
        uint8_t _maskRegister;
        uint8_t _statusRegister;
        uint8_t _oamAddressRegister;
        uint16_t _addressRegister;
        uint8_t _dataRegisterBuffer;

        bool _w = false;
        uint8_t _scrollX;
        uint8_t _scrollY;
        
        uint64_t _ppuCycles;
        uint64_t _scanLine;
        uint64_t _scanLineCycles;
        
        typedef std::array<Colour, 256*240> Frame;
        typedef std::array<uint8_t, 8*8> PaletteTile;

        Frame _frame;
        
    public:
        PPU();
        ~PPU();
        
        void setCartridge(std::shared_ptr<Cartridge> cartridge);
        
        void advanceClockAndCheckInterrupt(uint64_t cycles, bool& render, bool& nmiInterrupt);
        
        uint8_t readRegister_uint8(uint16_t address);
        void writeRegister_uint8(uint16_t address, uint8_t value);
        
        void oamDMA(std::array<uint8_t, 256> page);
        
        Frame getFrame() const;
        void renderFrame(uint16_t from, uint16_t to);
        Frame renderPatternTableToFrame();
        
    protected:
        void writeControlRegister(uint8_t input);
        void writeMaskRegister(uint8_t input);
        uint8_t readStatusRegister();
        void writeOAMAddressRegister(uint8_t input);
        uint8_t readOAMDataRegister();
        void writeOAMDataRegister(uint8_t input);
        void writeScrollRegister(uint8_t input);
        void writeAddressRegister(uint8_t input);
        uint8_t readDataRegister();
        void writeDataRegister(uint8_t input);
        
        uint8_t read_uint8(uint16_t address);
        void write_uint8(uint16_t address, uint8_t value);
        
    protected:
        void renderBackgroundTiles(Frame& frame, uint32_t nametable, Rect rect, Shift shift);
        void renderOAMTiles(Frame& frame, uint8_t priority);
        PaletteTile constructTile(const uint8_t* data);
        ColouredTile colourTile(uint8_t nametable,
                        uint8_t tileX,
                        uint8_t tileY,
                        std::array<uint8_t, 64>& tile);
        ColouredTile colourSprite(uint8_t palette,
                          std::array<uint8_t, 64>& tile);
        std::tuple<uint16_t, uint16_t, ColouredTile> getSprite(size_t index);
        
    private:
        uint16_t demirrorVRAMAddress(uint16_t address);
        uint16_t demirrorPaletteAddress(uint16_t address);
    };
}


#endif /* ppu_hpp */
