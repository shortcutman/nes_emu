//
//  instruction_utils.hpp
//  instructions
//
//  Created by Daniel Burke on 15/1/2022.
//

#ifndef instruction_utils_hpp
#define instruction_utils_hpp

#include <cstdint>

namespace nes_emu {

uint8_t statusFlagsOnByteValue(const uint8_t value, const uint8_t currentStatus);

}

#endif /* instruction_utils_hpp */
