//
//  registers.hpp
//  nes_emu
//
//  Created by Daniel Burke on 13/1/2022.
//

#ifndef registers_hpp
#define registers_hpp

#include <cstdint>

struct nes_registers {
    
    union {
        uint16_t programCounter;
        uint16_t pc;
    };
    
    union {
        uint8_t stackPointer;
        uint8_t sp;
    };
    
    union {
        uint8_t statusRegister;
        uint8_t p;
    };
    
    union {
        uint8_t accumulator;
        uint8_t a;
    };
    
    uint8_t x;
    uint8_t y;
    
};

#endif /* registers_hpp */
