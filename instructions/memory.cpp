//
//  memory.cpp
//  instructions
//
//  Created by Daniel Burke on 15/1/2022.
//

#include "memory.hpp"

#include "cartridge.hpp"

nes_emu::Memory::Memory() {
    
}

nes_emu::Memory::~Memory() {
    
}

uint8_t nes_emu::Memory::read_uint8(const uint16_t address) const {
    if (address <= nes_emu::Memory::MaxInternalRamMirrorAddress) {
        return _internalRAM[address & 0x07FF];
    } else if (address <= nes_emu::Memory::MaxPPURegisterMirrorAddress) {
        throw std::runtime_error("ppu not implemented");
        return 0x00;
    } else if (address <= nes_emu::Memory::LastAPUIORegister) {
        return _apuRegisters[address & 0x12];
    } else if (address <= nes_emu::Memory::LastAPUIOTestModeRegister) {
        throw std::runtime_error("apu/io test mode not implemented");
        return 0x00;
    } else if (address <= nes_emu::Memory::LastCartridgeSpaceMemory) {
        return _cartridge->readCart(address);;
    } else {
        throw std::logic_error("something is wrong");
        return 0x00;
    }
}

uint16_t nes_emu::Memory::read_uint16(const uint16_t address) const {    
    uint16_t loByte = read_uint8(address);
    uint16_t hiByte = read_uint8(address + 1);
    
    return hiByte << 8 | loByte;
}

void nes_emu::Memory::write(const uint16_t address, const uint8_t value) {
    if (address <= nes_emu::Memory::MaxInternalRamMirrorAddress) {
        _internalRAM[address & 0x07FF] = value;
    } else if (address <= nes_emu::Memory::MaxPPURegisterMirrorAddress) {
        throw std::runtime_error("ppu not implemented");
    } else if (address <= nes_emu::Memory::LastAPUIORegister) {
        _apuRegisters[address & 0x12] = value;
    } else if (address <= nes_emu::Memory::LastAPUIOTestModeRegister) {
        throw std::runtime_error("apu/io test mode not implemented");
    } else if (address <= nes_emu::Memory::LastCartridgeSpaceMemory) {
        throw std::runtime_error("catridge space not implemented");
    } else {
        throw std::logic_error("something is wrong");
    }
}
