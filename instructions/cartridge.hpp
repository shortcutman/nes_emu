//
//  cartridge.hpp
//  instructions
//
//  Created by Daniel Burke on 20/1/2022.
//

#ifndef cartridge_hpp
#define cartridge_hpp

#include <cstdint>
#include <memory>
#include <vector>

namespace nes_emu {

class Cartridge {
private:
    constexpr static const char iNesTag[4] = {0x4E, 0x45, 0x53, 0x1A};
    static const uint16_t PRGROMPageSize = 16384;
    static const uint16_t CHRROMPageSize = 8192;
    static const uint16_t PRGRAMPageSize = 8192;
    
private:
    uint8_t _prgRomBanks;
    uint8_t _chrRomBanks;
    uint8_t _controlByteOne;
    uint8_t _controlByteTwo;
    uint8_t _prgRamUnits;
    
    std::vector<uint8_t> _prgROM;
    std::vector<uint8_t> _chrROM;
    std::vector<uint8_t> _prgRAM;
    
public:
    Cartridge();
    ~Cartridge();
    
    static std::unique_ptr<Cartridge> cartridgeFromStream(std::istream& input);
};

}

#endif /* cartridge_hpp */
