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
        
        enum PPUStatus : uint8_t {
            SpriteOverflow = 0x20,
            SpriteZeroHit = 0x40,
            VerticalBlankStarted = 0x80
        };
        
    protected:
        std::shared_ptr<const Cartridge> _cartridge;
        std::array<uint8_t, 2048>  _vram;
        std::array<uint8_t, 32>  _paletteRAM;
        std::array<uint8_t, 256>  _oam;
        
        uint8_t _controlRegister;
        uint8_t _maskRegister;
        uint8_t _statusRegister;
        uint8_t _oamAddressRegister;
        uint16_t _addressRegister;
        uint8_t _dataRegisterBuffer;
        
        uint64_t _ppuCycles;
        uint64_t _scanLine;
        uint64_t _scanLineCycles;
        
        typedef std::tuple<uint8_t, uint8_t, uint8_t> Colour;
        typedef std::array<Colour, 256*240> Frame;
        
    public:
        PPU();
        ~PPU();
        
        void setCartridge(std::shared_ptr<const Cartridge> cartridge);
        
        void advanceClockAndCheckInterrupt(uint64_t cycles, bool& nmiInterrupt);
        
        uint8_t readRegister_uint8(uint16_t address);
        void writeRegister_uint8(uint16_t address, uint8_t value);
        
        void oamDMA(std::array<uint8_t, 256> page);
        
        Frame renderFrame();
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
        std::array<uint8_t, 64> constructTile(const uint8_t* data);
        std::array<Colour, 64> colourTile(std::array<uint8_t, 64>& tile);
        
    private:
        uint16_t demirrorVRAMAddress(uint16_t);
    };
}


#endif /* ppu_hpp */
