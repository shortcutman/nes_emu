//
//  cartridge.cpp
//  instructions
//
//  Created by Daniel Burke on 20/1/2022.
//

#include "cartridge.hpp"

#include <istream>
#include <iostream>
#include <array>
#include <algorithm>
#include <sstream>

nes_emu::Cartridge::Cartridge() {
    
}

nes_emu::Cartridge::~Cartridge() {
    
}

nes_emu::Cartridge::MirrorType nes_emu::Cartridge::mirrorType() const {
    if (_controlByteOne & 0x08) {
        return MirrorType::FourScreen;
    } else if (_controlByteOne & 0x01) {
        return MirrorType::Vertical;
    } else {
        return MirrorType::Horizontal;
    }
}

uint16_t nes_emu::Cartridge::prgROMSize() const {
    return _prgROM.size();
}

uint8_t nes_emu::Cartridge::readPRGROM(const uint16_t address) const {
    return _prgROM[address];
}

std::unique_ptr<nes_emu::Cartridge> nes_emu::Cartridge::cartridgeFromStream(std::istream& input) {
    auto cart = std::make_unique<nes_emu::Cartridge>();
    
    char magicWord[4];
    input.read(reinterpret_cast<char*>(&magicWord), sizeof(magicWord));
    
    if (*magicWord != *nes_emu::Cartridge::iNesTag) {
        throw std::runtime_error("File is not an iNES ROM");
        return nullptr;
    }
    
    std::array<uint8_t, 7> headerRemainder;
    headerRemainder.fill(0);
    
    input.read(reinterpret_cast<char*>(&cart->_prgRomBanks), sizeof(cart->_prgRomBanks))
    .read(reinterpret_cast<char*>(&cart->_chrRomBanks), sizeof(cart->_chrRomBanks))
    .read(reinterpret_cast<char*>(&cart->_controlByteOne), sizeof(cart->_controlByteOne))
    .read(reinterpret_cast<char*>(&cart->_controlByteTwo), sizeof(cart->_controlByteTwo))
    .read(reinterpret_cast<char*>(&cart->_prgRamUnits), sizeof(cart->_prgRamUnits))
    .read(reinterpret_cast<char*>(&headerRemainder), sizeof(headerRemainder.size()));
    
    if (!input.good()) {
        throw std::runtime_error("");
    }
    
    if (std::any_of(headerRemainder.begin(), headerRemainder.end(), [](uint8_t i) { return i != 0; })) {
        std::runtime_error("End of header not zeroes");
    }
    
    if ((cart->_controlByteOne & 0xF0) != 0 ||
        (cart->_controlByteTwo & 0xF0) != 0) {
        throw std::runtime_error("unsupported mapper");
    } else if ((cart->_controlByteOne & 0b00000100) != 0) {
        throw std::runtime_error("trainers unsupported");
    } else if ((cart->_controlByteOne & 0b00000010) != 0) {
        throw std::runtime_error("battery backed ram unsupported");
    } else if ((cart->_controlByteTwo & 0x0F) != 0) {
        throw std::runtime_error("ines 2.0 unsupported");
    }
    
    uint16_t prgROMSize = PRGROMPageSize * cart->_prgRomBanks;
    cart->_prgROM.assign(prgROMSize, 0);
    input.read(reinterpret_cast<char*>(cart->_prgROM.data()), prgROMSize);
    
    if (!input.good() || input.gcount() != prgROMSize) {
        std::stringstream ss;
        ss  << "Failed to read PRG ROM, expected:"
            << prgROMSize
            << "but read"
            << input.gcount();
        throw std::runtime_error(ss.str());
    }
    
    uint16_t chrROMSize = CHRROMPageSize * cart->_chrRomBanks;
    cart->_chrROM.assign(chrROMSize, 0);
    input.read(reinterpret_cast<char*>(cart->_chrROM.data()), chrROMSize);
    
    if (!input.good() && (input.gcount() == 0 && chrROMSize > 0)) {
        std::stringstream ss;
        ss  << "Failed to read CHR ROM, expected:"
            << chrROMSize
            << "but read"
            << input.gcount();
        throw std::runtime_error(ss.str());
    }

    uint16_t prgRAMSize = PRGRAMPageSize * cart->_prgRamUnits;
    cart->_prgRAM.assign(prgRAMSize, 0);
    input.read(reinterpret_cast<char*>(cart->_prgRAM.data()), prgRAMSize);
    
    if (!input.good() && (input.gcount() == 0 && prgRAMSize > 0)) {
        std::stringstream ss;
        ss  << "Failed to read PRG RAM, expected:"
            << prgRAMSize
            << "but read"
            << input.gcount();
        throw std::runtime_error(ss.str());
    }
    
    return cart;
}
