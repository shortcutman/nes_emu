//
//  memory.cpp
//  instructions
//
//  Created by Daniel Burke on 15/1/2022.
//

#include "memory.hpp"

#include "cartridge.hpp"
#include "ppu.hpp"

nes_emu::Memory::Memory() :
_ppu(new PPU),
_cpuClock(0),
_ppuClock(0)
{
    
}

nes_emu::Memory::~Memory() {
    
}

void nes_emu::Memory::setCartridge(std::shared_ptr<Cartridge> cartridge) {
    _cartridge = cartridge;
}

void nes_emu::Memory::setPPU(std::shared_ptr<PPU> ppu) {
    _ppu = ppu;
}

uint64_t nes_emu::Memory::cpuClock() const {
    return _cpuClock;
}

void nes_emu::Memory::advanceClock(uint64_t cycles) {
    _cpuClock += cycles;
    
    auto ppuCycleIncrement = cycles * 3;
    _ppuClock += ppuCycleIncrement;
    
    bool nmiInterrupt = false;
    _ppu->advanceClockAndCheckInterrupt(ppuCycleIncrement, nmiInterrupt);
    
    if (nmiInterrupt) {
        _interruptCallback();
    }
}

void nes_emu::Memory::setInterruptCallback(std::function<void(void)> func) {
    _interruptCallback = std::move(func);
}


uint8_t nes_emu::Memory::read_uint8(const uint16_t address) const {
    if (address <= nes_emu::Memory::MaxInternalRamMirrorAddress) {
        return _internalRAM[address & 0x07FF];
    } else if (address <= nes_emu::Memory::MaxPPURegisterMirrorAddress) {
        return _ppu->readRegister_uint8(address);
    } else if (address <= nes_emu::Memory::LastAPUIORegister) {
        return _apuRegisters[address & 0x12];
    } else if (address <= nes_emu::Memory::LastAPUIOTestModeRegister) {
        throw std::runtime_error("apu/io test mode not implemented");
        return 0x00;
    } else if (address <= nes_emu::Memory::LastCartridgeSpaceMemory) {
        return _cartridge->readCart(address);
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

std::array<uint8_t, 256> nes_emu::Memory::readPage(uint8_t pageAddress) const {
    uint16_t startAddress = static_cast<uint16_t>(pageAddress) << 8;
    std::array<uint8_t, 256> page;
    
    *page.data() = _internalRAM[startAddress];
    return page;
}

void nes_emu::Memory::write(const uint16_t address, const uint8_t value) {
    if (address <= nes_emu::Memory::MaxInternalRamMirrorAddress) {
        _internalRAM[address & 0x07FF] = value;
    } else if (address <= nes_emu::Memory::MaxPPURegisterMirrorAddress) {
        _ppu->writeRegister_uint8(address, value);
    } else if (address <= nes_emu::Memory::LastAPUIORegister) {
        if (address == 0x4014) {
#warning CPU should suspend for 513 or 514 cycles
            _ppu->oamDMA(readPage(value));
        } else {
            _apuRegisters[address & 0x12] = value;
        }
    } else if (address <= nes_emu::Memory::LastAPUIOTestModeRegister) {
        throw std::runtime_error("apu/io test mode not implemented");
    } else if (address <= nes_emu::Memory::LastCartridgeSpaceMemory) {
        throw std::runtime_error("catridge space not implemented");
    } else {
        throw std::logic_error("something is wrong");
    }
}
