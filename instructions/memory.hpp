//
//  memory.hpp
//  instructions
//
//  Created by Daniel Burke on 15/1/2022.
//

#ifndef memory_hpp
#define memory_hpp

#include <array>

namespace nes_emu {
    class Memory {
        
    private:
        const uint16_t InternalRamSize = 0x0800;
        const uint16_t MaxInternalRamMirrorAddress = 0x1FFF;
        const uint16_t FirstPPURegister = 0x2000;
        const uint16_t LastPPURegister = 0x2007;
        const uint16_t MaxPPURegisterMirrorAddress = 0x3FFF;
        const uint16_t FirstAPUIORegister = 0x4000;
        const uint16_t LastAPUIORegister = 0x4017;
        const uint16_t LastAPUIOTestModeRegister = 0x401F;
        const uint16_t LastCartridgeSpaceMemory = 0xFFFF;

        
    private:
        std::array<uint8_t, 2048> _internalRAM;
        std::array<uint8_t, 8> _ppuRegisters;
        
    public:
        Memory();
        ~Memory();
        
        uint8_t read_uint8(const uint16_t address) const;
        uint16_t read_uint16(const uint16_t address) const;
        
        void write(const uint16_t address, const uint8_t value);
        
        static const uint16_t StackStart = 0x0100;
    };
}

#endif /* memory_hpp */
