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
    };
    Pulse _pulse1;

    bool _enableDMC = false;
    bool _enableNoise = false;
    bool _enableTriangle = false;
    bool _enablePulse1 = false;
    bool _enablePulse2 = false;

    uint16_t _mode = 0;

public:
    APU() {}
    ~APU() {}

    void advanceClock(uint64_t cycles);

    void writeRegister(const uint16_t address, const uint8_t value);

protected:
    void writePulse1(const uint16_t address, const uint8_t value);
    void writeStatus(const uint8_t value);
    void writeFrameCounter(const uint8_t value);
};

}

#endif