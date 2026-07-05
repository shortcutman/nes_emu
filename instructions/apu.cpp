//
//  apu.cpp
//  instructions
//
//  Created by Daniel Burke on 5/7/2026.
//

#include "apu.hpp"

#include <cassert>

void nes_emu::APU::writeRegister(const uint16_t address, const uint8_t value) {
    assert(address >= 0x4000 && address < 0x4018);

    if (address < 0x4004) {
        writePulse1(address, value);
    } else if (address == 0x4015) {
        writeStatus(value);
    } else if (address == 0x4017) {
        writeFrameCounter(value);
    }
}

void nes_emu::APU::writePulse1(const uint16_t address, const uint8_t value) {
    switch (address & 0x3) {
        case 0:
            _pulse1._duty = value >> 6;
            _pulse1._lengthCounterHalt = (value >> 5) & 0x1;
            _pulse1._constantVolume = (value >> 4) & 0x1;
            _pulse1._volume = value & 0xf;
        break;

        case 1:
            _pulse1._sweepEnabled = value >> 7;
            _pulse1._sweepPeriod = (value >> 4) & 0x7;
            _pulse1._sweepNegate = (value >> 3) & 0x1;
            _pulse1._sweepShift = value & 0x7;
        break;

        case 2:
            _pulse1._timer = (_pulse1._timer & 0xff00) | value;
        break;

        case 3:
            _pulse1._lengthCounterLoad = value >> 3;
            _pulse1._timer = (_pulse1._timer & 0xff) | (static_cast<uint16_t>(value & 0x7) << 8);
        break;
    }
}

void nes_emu::APU::writeStatus(const uint8_t value) {
    _enableDMC = value & 0x10 ? true : false;
    _enableNoise = value & 0x08 ? true : false;
    _enableTriangle = value & 0x04 ? true : false;
    _enablePulse1 = value & 0x02 ? true : false;
    _enablePulse2 = value & 0x01 ? true : false;
}

void nes_emu::APU::writeFrameCounter(const uint8_t value) {
    _mode = value >> 7;
}