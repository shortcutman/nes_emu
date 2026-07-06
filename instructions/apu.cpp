//
//  apu.cpp
//  instructions
//
//  Created by Daniel Burke on 5/7/2026.
//

#include "apu.hpp"

#include <cassert>

namespace {
    std::array<std::array<uint8_t, 8>, 4> PulseDutySequence = {{
        {0, 1, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0, 0, 0},
        {1, 0, 0, 1, 1, 1, 1, 1}
    }};

    std::array<std::array<uint32_t, 4>, 2> FrameCounterMode = {{
        {3728, 7456, 11185, 14914},
        {3728, 7456, 11185, 18640}
    }};

    std::array<uint8_t, 32> LengthCounterLoad = {
        10, 254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
        12,  16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
    };
}

void nes_emu::APU::Envelope::step() {
    if (!_startFlag) {
        if (_divider == 0) {
            _divider = _volume;

            if (_decayLevelCounter != 0) {
                _decayLevelCounter--;
            } else if (_loop) {
                _decayLevelCounter = 15;
            }
        } else {
            _divider--;
        }
    } else {
        _startFlag = false;
        _decayLevelCounter = 15;
        _divider = _volume;
    }
}

uint8_t nes_emu::APU::Envelope::volume() {
    return _constantVolume ? _volume : _decayLevelCounter;
}

void nes_emu::APU::Pulse::step(int stepNum) {
    _envelope.step();

    //length counter
    if ((stepNum == 1 || stepNum == 3) && !_lengthCounterHalt && _lengthCounter > 0) {
        _lengthCounter--;
    }

    //sweep
}

void nes_emu::APU::Pulse::tick(uint64_t cycles) {
    _timerState -= cycles;
    if (_timerState <= 0) {
        _dutySequenceState++;
        _timerState = _timer; //maybe a problem
    }
}

int16_t nes_emu::APU::Pulse::sample() {
    if (_lengthCounter > 0 && _timerState > 8) {
        return PulseDutySequence[_duty][_dutySequenceState % 8] * _envelope.volume();
    } else {
        return 0;
    }
}

void nes_emu::APU::advanceClock(uint64_t cycles) {
    if (carry) {
        cycles++;
    }
    carry = cycles % 2 ? true : false;
    uint64_t apuDeltaCycles = cycles / 2;
    _cycles += apuDeltaCycles;

    if (_cycles >= FrameCounterMode[_mode][_frameCounterStep]) {
        //envelopes

        _pulse1.step(_frameCounterStep);

        _frameCounterStep++;
        if (_frameCounterStep >= 4) {
            _cycles = 0;
            _frameCounterStep = 0;
        }
    }

    _pulse1.tick(apuDeltaCycles);

    sample_cycles += cycles;
    if (sample_cycles >= cycles_per_sample) {
        sample_cycles -= cycles_per_sample;

        if (sample_counter < max_samples) {
            uint8_t pulse_sample = _pulse1.sample();
            float mixed_sample = 0.f;
            if (pulse_sample != 0) {
                mixed_sample = 95.52f / ((8128.f / pulse_sample) + 100.f) * std::numeric_limits<int16_t>::max();
            }

            _samples[sample_counter++] = mixed_sample;
        }
    }
}

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
            _pulse1._envelope._constantVolume = (value >> 4) & 0x1;
            _pulse1._envelope._volume = value & 0xf;
            _pulse1._envelope._loop = _pulse1._lengthCounterHalt;
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
            _pulse1._lengthCounter = LengthCounterLoad[_pulse1._lengthCounterLoad];
            _pulse1._timer = (_pulse1._timer & 0xff) | (static_cast<uint16_t>(value & 0x7) << 8);
            _pulse1._dutySequenceState = 0;
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

    _cycles = 0;
    _frameCounterStep = 0;
}