//
//  apu.hpp
//  instructions
//
//  Created by Daniel Burke on 5/7/2026.
//

#ifndef apu_hpp
#define apu_hpp

#include <cstdint>
#include <memory>
#include <array>

namespace nes_emu {

class APU {
protected:
    struct Pulse {
        uint16_t _duty = 0;
        uint16_t _lengthCounterHalt = 0;
        uint16_t _constantVolume = 0;
        uint16_t _volume = 0;

        uint16_t _sweepEnabled = 0;
        uint16_t _sweepPeriod = 0;
        uint16_t _sweepNegate = 0;
        uint16_t _sweepShift = 0;

        uint16_t _timer = 0;
        uint16_t _lengthCounterLoad = 0;

        int16_t _timerState;
        uint16_t _dutySequenceState;
        uint16_t _lengthCounter = 0;
    };
    Pulse _pulse1;

    bool _enableDMC = false;
    bool _enableNoise = false;
    bool _enableTriangle = false;
    bool _enablePulse1 = false;
    bool _enablePulse2 = false;

    uint16_t _mode = 0;
    uint16_t _frameCounterStep = 0;
    uint64_t _cycles = 0;
    bool carry = false;

    static constexpr float sample_rate = 40100.f;
    static constexpr float cycles_per_sample = 1789773.f / sample_rate;
    static const int max_samples = sample_rate / 60 * 3;

public:
    APU() {}
    ~APU() {}

    float sample_cycles = 0;
    uint32_t sample_counter = 0;
    std::array<int16_t, max_samples> _samples;

    void clear_samples() { sample_counter = 0; }

    void advanceClock(uint64_t cycles);

    void writeRegister(const uint16_t address, const uint8_t value);

protected:
    void writePulse1(const uint16_t address, const uint8_t value);
    void writeStatus(const uint8_t value);
    void writeFrameCounter(const uint8_t value);
};

}

#endif