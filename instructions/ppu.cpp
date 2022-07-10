//
//  ppu.cpp
//  instructions
//
//  Created by Daniel Burke on 4/2/2022.
//

#include "ppu.hpp"

#include <stdexcept>

#include "cartridge.hpp"

namespace {
const uint64_t PPUCyclesPerScanLine = 341;
const uint64_t PPUNMITriggerScanLine = 241;
}

nes_emu::PPU::PPU() :
    _controlRegister(0),
    _maskRegister(0),
    _statusRegister(0),
    _oamAddressRegister(0),
    _addressRegister(0),
    _dataRegisterBuffer(0),
    _ppuCycles(0),
    _scanLine(0),
    _scanLineCycles(0)
{
    _vram.fill(0);
    _paletteRAM.fill(0);
    _oam.fill(0);
}

nes_emu::PPU::~PPU() {
    
}

void nes_emu::PPU::setCartridge(std::shared_ptr<const Cartridge> cartridge) {
    _cartridge = cartridge;
}

void nes_emu::PPU::advanceClockAndCheckInterrupt(uint64_t cycles, bool& nmiInterrupt) {
    nmiInterrupt = false;
    _ppuCycles += cycles;
    _scanLineCycles += cycles;
    
    if (_scanLineCycles >  (PPUCyclesPerScanLine * 242)) {
        throw std::runtime_error("clock will advance without triggering NMI interrupt");
    }
    
    if (_scanLineCycles >= PPUCyclesPerScanLine) {
        _scanLine += _scanLineCycles / PPUCyclesPerScanLine;
        _scanLineCycles = _scanLineCycles % PPUCyclesPerScanLine;
        
        if (_scanLine == PPUNMITriggerScanLine) {
            nmiInterrupt = true;
            _statusRegister |= 0x80;
        } else if (_scanLine >= 262) {
            _scanLine = 0;
            _statusRegister &= 0x7F;
        }
    }
}

uint8_t nes_emu::PPU::readRegister_uint8(uint16_t address) {
    if (address < 0x2000 || address > 0x3FFF) {
        throw std::logic_error("Address does not relate to PPU");
        return 0;
    }
    
    address &= 0x07;
    
    switch (address) {
        case 0x02:
            return readStatusRegister();
            break;

        case 0x04:
            return readOAMDataRegister();
            break;
                        
        case 0x07:
            return readDataRegister();
            break;
            
        default:
            throw std::logic_error("Called register that cannot be read");
            return 0;
            break;
    }
}

void nes_emu::PPU::writeRegister_uint8(uint16_t address, uint8_t value) {
    if (address < 0x2000 || address > 0x3FFF) {
        throw std::logic_error("Address does not relate to PPU");
    }
    
    address &= 0x07;
    
    switch (address) {
        case 0x00:
            writeControlRegister(value);
            break;

        case 0x01:
            writeMaskRegister(value);
            break;
                        
        case 0x03:
            writeOAMAddressRegister(value);
            break;
            
        case 0x04:
            writeOAMDataRegister(value);
            break;
            
        case 0x05:
            writeScrollRegister(value);
            break;
            
        case 0x06:
            writeAddressRegister(value);
            break;
            
        case 0x07:
            writeDataRegister(value);
            break;
            
        default:
            throw std::logic_error("Called register that cannot be read");
            break;
    }
}

void nes_emu::PPU::oamDMA(std::array<uint8_t, 256> page) {
    _oam = std::move(page);
}

void nes_emu::PPU::writeControlRegister(uint8_t input) {
    if (_ppuCycles < 30000) {
        return;
    }
    
    _controlRegister = input;
}

void nes_emu::PPU::writeMaskRegister(uint8_t input) {
    _maskRegister = input;
}

uint8_t nes_emu::PPU::readStatusRegister() {
    auto result = _statusRegister;
    _statusRegister &= 0x7F; //vsync flag cleared after read
    return result;
}

void nes_emu::PPU::writeOAMAddressRegister(uint8_t input) {
    _oamAddressRegister = input;
}

uint8_t nes_emu::PPU::readOAMDataRegister() {
    return _oam[_oamAddressRegister];
}

void nes_emu::PPU::writeOAMDataRegister(uint8_t input) {
    _oam[_oamAddressRegister] = input;
    _oamAddressRegister++;
}

void nes_emu::PPU::writeScrollRegister(uint8_t input) {
    
}

void nes_emu::PPU::writeAddressRegister(uint8_t input) {
    uint16_t newAddress = input;
    newAddress += _addressRegister << 8;
    _addressRegister = newAddress;
}

uint8_t nes_emu::PPU::readDataRegister() {
    auto dataBufferReturn = _dataRegisterBuffer;
    
    _dataRegisterBuffer = read_uint8(_addressRegister);
    if (_controlRegister & 0b00000010) {
        _addressRegister += 32;
    } else {
        _addressRegister += 1;
    }
    
    return dataBufferReturn;
}

void nes_emu::PPU::writeDataRegister(uint8_t input) {
    write_uint8(_addressRegister, input);
    
    if (_controlRegister & 0b00000010) {
        _addressRegister += 32;
    } else {
        _addressRegister += 1;
    }
}

uint8_t nes_emu::PPU::read_uint8(uint16_t address) {
    if (address < 0x2000) {
        return _cartridge->readCHRRom(address);
    } else if (address < 0x3EFF) {
        return _vram[demirrorVRAMAddress(address)];
    } else if (address < 0x3FFF) {
        auto addressOffset = (address - 0x3EFF) & 0x1F;
        return _paletteRAM[addressOffset];
    }
    
    throw std::logic_error("no memory at address");
    return 0;
}

void nes_emu::PPU::write_uint8(uint16_t address, uint8_t value) {
    if (address < 0x2000) {
        throw std::logic_error("PPU CHRROM not writable");
    } else if (address < 0x3000) {
        _vram[demirrorVRAMAddress(address)] = value;
    } else if (address < 0x3FFF) {
        auto addressOffset = (address - 0x3EFF) & 0x1F;
        _paletteRAM[addressOffset] = value;
    }
    
    throw std::logic_error("no PPU memory at address");
}

uint16_t nes_emu::PPU::demirrorVRAMAddress(uint16_t address) {
    if (address < 0x2000 || address > 0x3F00) {
        throw std::logic_error("not a PPU VRAM memory address");
        return 0;
    } else if (address > 0x2FFF && address < 0x3F00) {
        address -= 0x1000;
    }
    
    auto vramAddress = address - 0x2000;
    auto vramMirrorBlock = vramAddress / 0x400;
    
    switch (_cartridge->mirrorType()) {
        case nes_emu::Cartridge::MirrorType::Horizontal:
        {
            if (vramMirrorBlock == 1 ||
                vramMirrorBlock == 2) {
                vramAddress -= 0x400;
            } else if (vramMirrorBlock == 3) {
                vramAddress -= 0x800;
            }
        }
            break;
            
        case nes_emu::Cartridge::MirrorType::Vertical:
        {
            if (vramMirrorBlock == 2 ||
                vramMirrorBlock == 3) {
                vramAddress -= 0x800;
            }
        }
            break;
            
        default:
            throw std::logic_error("not implemented");
            break;
    }
    
    return vramAddress;
}
