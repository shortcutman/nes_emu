//
//  memory.cpp
//  instructions
//
//  Created by Daniel Burke on 15/1/2022.
//

#include "memory.hpp"

#include "apu.hpp"
#include "cartridge.hpp"
#include "input_controller.hpp"
#include "ppu.hpp"

#include <algorithm>
#include <iterator>

nes_emu::Memory::Memory() :
_ppu(new PPU),
_cpuClock(0),
_ppuClock(0)
{
    _internalRAM.fill(0);
}

nes_emu::Memory::~Memory() {
    
}

void nes_emu::Memory::setCartridge(std::shared_ptr<Cartridge> cartridge) {
    _cartridge = cartridge;
}

void nes_emu::Memory::setController(std::shared_ptr<Controller> controller) {
    _controller = controller;
}

void nes_emu::Memory::setPPU(std::shared_ptr<PPU> ppu) {
    _ppu = ppu;
}

void nes_emu::Memory::setAPU(std::shared_ptr<APU> apu) {
    _apu = apu;
}

uint64_t nes_emu::Memory::cpuClock() const {
    return _cpuClock;
}

uint64_t nes_emu::Memory::ppuClock() const {
    return _ppuClock;
}

void nes_emu::Memory::advanceClock(uint64_t cycles) {
    _cpuClock += cycles;
    
    auto ppuCycleIncrement = cycles * 3;
    _ppuClock += ppuCycleIncrement;
    
    bool render = false, nmiInterrupt = false;
    _ppu->advanceClockAndCheckInterrupt(ppuCycleIncrement, render, nmiInterrupt);

    _apu->advanceClock(cycles);

    if (render) {
        _gameLoopCallback();
    }
    
    if (nmiInterrupt) {
        _nmiInterruptCallback();
    }
}

void nes_emu::Memory::setNMIInterruptCallback(std::function<void(void)> func) {
    _nmiInterruptCallback = std::move(func);
}

void nes_emu::Memory::setGameLoopCallback(std::function<void(void)> func) {
    _gameLoopCallback = std::move(func);
}

uint8_t nes_emu::Memory::read_uint8(const uint16_t address) const {
    if (address <= nes_emu::Memory::MaxInternalRamMirrorAddress) {
        return _internalRAM[address & 0x07FF];
    } else if (address <= nes_emu::Memory::MaxPPURegisterMirrorAddress) {
        return _ppu->readRegister_uint8(address);
    } else if (address == 0x4016) {
        return _controller->readJoy1();
    } else if (address == 0x4017) {
        return _controller->readJoy2();
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
    
    std::copy_n(
                std::next(std::begin(_internalRAM), startAddress),
                page.size(),
                std::begin(page));
    
    return page;
}

void nes_emu::Memory::write(const uint16_t address, const uint8_t value) {
    if (address <= nes_emu::Memory::MaxInternalRamMirrorAddress) {
        _internalRAM[address & 0x07FF] = value;
    } else if (address <= nes_emu::Memory::MaxPPURegisterMirrorAddress) {
        _ppu->writeRegister_uint8(address, value);
    } else if (address <= nes_emu::Memory::LastAPUIORegister) {
        if (address == 0x4014) {
            //TODO: CPU should suspend for 513 or 514 cycles
            _ppu->oamDMA(readPage(value));
            advanceClock(513);
        } else if (address == 0x4016) {
            _controller->writeStrobe(value);
        } else {
            _apu->writeRegister(address, value);
            // _apuRegisters[address & 0x12] = value;
        }
    } else if (address <= nes_emu::Memory::LastAPUIOTestModeRegister) {
        throw std::runtime_error("apu/io test mode not implemented");
    } else if (address <= nes_emu::Memory::LastCartridgeSpaceMemory) {
        throw std::runtime_error("catridge space not implemented");
    } else {
        throw std::logic_error("something is wrong");
    }
}
