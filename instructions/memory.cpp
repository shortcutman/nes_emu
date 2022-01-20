//
//  memory.cpp
//  instructions
//
//  Created by Daniel Burke on 15/1/2022.
//

#include "memory.hpp"

nes_memory::nes_memory() {
    
}

nes_memory::~nes_memory() {
    
}

uint8_t nes_memory::read_uint8(const uint16_t address) const {
    if (address <= nes_memory::MaxInternalRamMirrorAddress) {
        return _internalRAM[address & 0x08FF];
    } else if (address <= nes_memory::MaxPPURegisterMirrorAddress) {
        throw std::runtime_error("ppu not implemented");
        return 0x00;
    } else if (address <= nes_memory::LastAPUIORegister) {
        throw std::runtime_error("apu/io not implemented");
        return 0x00;
    } else if (address <= nes_memory::LastAPUIOTestModeRegister) {
        throw std::runtime_error("apu/io test mode not implemented");
        return 0x00;
    } else if (address <= nes_memory::LastCartridgeSpaceMemory) {
        throw std::runtime_error("catridge space not implemented");
        return 0x00;
    } else {
        throw std::logic_error("something is wrong");
        return 0x00;
    }
}

uint16_t nes_memory::read_uint16(const uint16_t address) const {
    assert(address < 2048);
    
    uint16_t loByte = read_uint8(address);
    uint16_t hiByte = read_uint8(address + 1);
    
    return hiByte << 8 | loByte;
}

void nes_memory::write(const uint16_t address, const uint8_t value) {
    if (address <= nes_memory::MaxInternalRamMirrorAddress) {
        _internalRAM[address & 0x08FF] = value;
    } else if (address <= nes_memory::MaxPPURegisterMirrorAddress) {
        throw std::runtime_error("ppu not implemented");
    } else if (address <= nes_memory::LastAPUIORegister) {
        throw std::runtime_error("apu/io not implemented");
    } else if (address <= nes_memory::LastAPUIOTestModeRegister) {
        throw std::runtime_error("apu/io test mode not implemented");
    } else if (address <= nes_memory::LastCartridgeSpaceMemory) {
        throw std::runtime_error("catridge space not implemented");
    } else {
        throw std::logic_error("something is wrong");
    }
}
