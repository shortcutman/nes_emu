//
//  apu.cpp
//  instructions
//
//  Created by Daniel Burke on 5/7/2026.
//

#include "apu.hpp"

#include <cassert>
#include <cmath>

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

        if (sweepDivider == 0 && _sweepEnabled && _sweepShift != 0) {
            calcTargetPeriod();
            if (targetPeriod < 0x8ff && _timer >= 8) {
                _timer = targetPeriod;
            }
        }
        
        if (sweepDivider == 0 || sweepReload) {
            sweepDivider = _sweepPeriod;
            sweepReload = false;
        } else {
            sweepDivider--;
        }
    }
}

int32_t nes_emu::APU::Pulse::apuSample() {
    if (_lengthCounter != 0 && _timer >= 8 && targetPeriod < 0x8ff) {
        return PulseDutySequence[_duty][_dutySequenceState] * _envelope.volume();
    } else {
        return 0;
    }
}

void nes_emu::APU::Pulse::tick() {
    _timerState--;
    if (_timerState <= 0) {
        _dutySequenceState = (_dutySequenceState + 1) & 0x7;
        _timerState = _timer + _timerState;
    }
}

//https://stackoverflow.com/questions/23621435/how-to-determine-alpha-smoothing-constant-of-a-lowpass-filter
constexpr float alpha() {
    double dt = 1.0 / nes_emu::APU::sample_rate;
    double rc = 1.0 / (2.0 * M_PI * 22050);
    return dt / (rc + dt);
}

int16_t nes_emu::APU::Pulse::sample() {
    auto nextXSample = accumulate / accumlatedSamples;
    accumulate = 0;
    accumlatedSamples = 0;
    return nextXSample;

    // https://en.wikipedia.org/wiki/Exponential_smoothing
    // int16_t sample = alpha() * nextXSample + (1.f - alpha()) * lastSample;
    // lastSample = sample;
    // return sample;
}

void nes_emu::APU::Pulse::calcTargetPeriod() {
    int16_t changeAmount = _timer >> _sweepShift;
    if (_sweepNegate) {
        changeAmount *= -1;
    }
    targetPeriod = _timer + changeAmount;
}

void nes_emu::APU::advanceClock(const uint64_t cpuCyclesDelta) {
    auto cpuCyclesDeltaWithCarry = cpuCyclesDelta;
    _apuCycles += cpuCyclesDeltaWithCarry / 2;

    if (_apuCycles >= FrameCounterMode[_mode][_frameCounterStep]) {
        _pulse1.step(_frameCounterStep);
        _pulse2.step(_frameCounterStep);

        _frameCounterStep++;
        if (_frameCounterStep >= 4) {
            _apuCycles = 0;
            _frameCounterStep = 0;
        }
    }

    for (auto i = 0; i < cpuCyclesDelta; i++) {
        if (flip) {
            _pulse1.tick();
            _pulse2.tick();
        }
        flip = !flip;

        sample_cycles++;

        if (sample_cycles < cycles_per_sample) {
            _pulse1.accumulate += _pulse1.apuSample();
            _pulse1.accumlatedSamples++;
            _pulse2.accumulate += _pulse2.apuSample();
            _pulse2.accumlatedSamples++;
        } else {
            auto fraction = sample_cycles - cycles_per_sample;

            auto pulse1apuSample = _pulse1.apuSample();
            _pulse1.accumulate += pulse1apuSample * (1 - fraction);
            _pulse1.accumlatedSamples += (1 - fraction);
            auto pulse2apuSample = _pulse1.apuSample();
            _pulse2.accumulate += pulse2apuSample * (1 - fraction);
            _pulse2.accumlatedSamples += (1 - fraction);

            uint8_t pulse_sample = _pulse1.sample() + _pulse2.sample();
            float mixed_sample = 0.f;
            if (pulse_sample != 0) {
                mixed_sample = 95.52f / ((8128.f / pulse_sample) + 100.f) * std::numeric_limits<int16_t>::max();
            }

            _samples[sample_counter++] = mixed_sample;

            _pulse1.accumulate = pulse1apuSample * fraction;
            _pulse1.accumlatedSamples = fraction;
            _pulse2.accumulate = pulse2apuSample * fraction;
            _pulse2.accumlatedSamples = fraction;

            sample_cycles = 0;
        }
    }
}

void nes_emu::APU::writeRegister(const uint16_t address, const uint8_t value) {
    assert(address >= 0x4000 && address < 0x4018);

    if (address < 0x4004) {
        _pulse1.write(address & 0x03, value);
    } else if (address < 0x4008) {
        _pulse2.write(address & 0x03, value);
    } else if (address == 0x4015) {
        writeStatus(value);
    } else if (address == 0x4017) {
        writeFrameCounter(value);
    }
}

void nes_emu::APU::Pulse::write(uint16_t address, uint8_t value) {
    switch (address) {
        case 0:
            _duty = (value >> 6) & 0x3;
            _lengthCounterHalt = (value >> 5) & 0x1;
            _envelope._constantVolume = (value >> 4) & 0x1;
            _envelope._volume = value & 0xf;
            _envelope._loop = _lengthCounterHalt;
        break;

        case 1:
            _sweepEnabled = value >> 7;
            _sweepPeriod = (value >> 4) & 0x7;
            _sweepNegate = (value >> 3) & 0x1;
            _sweepShift = value & 0x7;
            sweepReload = true;
        break;

        case 2:
            _timer = (_timer & 0xff00) | value;
        break;

        case 3:
            _lengthCounterLoad = value >> 3;
            _lengthCounter = LengthCounterLoad[_lengthCounterLoad];
            _timer = (_timer & 0xff) | (static_cast<uint16_t>(value & 0x7) << 8);
            _dutySequenceState = 0;
            _envelope._startFlag = true;
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

    _apuCycles = 0;
    _frameCounterStep = 0;
}