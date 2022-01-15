//
//  instruction_utils.cpp
//  instructions
//
//  Created by Daniel Burke on 15/1/2022.
//

#include "instruction_utils.hpp"

uint8_t nes_emu::statusFlagsOnByteValue(const uint8_t value, const uint8_t currentStatus) {
    uint8_t updatedStatus = currentStatus;
    
    if (value == 0) {
        updatedStatus |= 0b00000010;
    } else {
        updatedStatus &= 0b11111101;
    }
    
    if ((value & 0b10000000) != 0) {
        updatedStatus |= 0b10000000;
    } else {
        updatedStatus &= 0b01111111;
    }
    
    return updatedStatus;
}
