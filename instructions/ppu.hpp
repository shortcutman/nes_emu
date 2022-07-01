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
        std::shared_ptr<Cartridge> _cartridge;
        std::array<uint8_t, 2048>  _vram;
        std::array<uint8_t, 32>  _paletteRAM;
        std::array<uint8_t, 256>  _oam;
        
        uint8_t _controlRegister;
        uint8_t _maskRegister;
        uint8_t _oamAddressRegister;
        uint16_t _addressRegister;
        uint8_t _dataRegisterBuffer;
        
        uint16_t _scanLine;
        uint16_t _scanLineCycles;
        
    public:
        PPU();
        ~PPU();
        
        void advanceClockAndCheckInterrupt(uint64_t cycles, bool& nmiInterrupt);
        
        uint8_t readRegister_uint8(uint16_t address);
        void writeRegister_uint8(uint16_t address, uint8_t value);
        
        void oamDMA(std::array<uint8_t, 256> page);
        
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
        
    private:
        uint16_t demirrorVRAMAddress(uint16_t);
    };
}


#endif /* ppu_hpp */
