//
//  memory.hpp
//  instructions
//
//  Created by Daniel Burke on 15/1/2022.
//

#ifndef memory_hpp
#define memory_hpp

#include <array>

class nes_memory {
private:
    std::array<uint8_t, 2048> _internalRAM;
    
public:
    nes_memory();
    ~nes_memory();
    
    uint8_t read_uint8(const uint16_t address) const;
    uint16_t read_uint16(const uint16_t address) const;
    
    void write(const uint16_t address, const uint8_t value);
    
    static const uint16_t StackStart = 0x0100;
};

#endif /* memory_hpp */
