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
    assert(address < 2048);
    return _internalRAM[address];
}

uint16_t nes_memory::read_uint16(const uint16_t address) const {
    assert(address < 2048);
    
    uint16_t loByte = read_uint8(address);
    uint16_t hiByte = read_uint8(address + 1);
    
    return hiByte << 8 | loByte;
}

void nes_memory::write(const uint16_t address, const uint8_t value) {
    assert(address < 2048);
    _internalRAM[address] = value;
}
