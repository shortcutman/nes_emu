//
//  input_controller.hpp
//
//  Created by Daniel Burke on 09/5/2026.
//

#pragma once

#include <cstdint>
#include <functional>

namespace nes_emu {
    class Controller {
    public:
        enum Button : uint8_t {
            A = 0,
            B = 1,
            Select = 2,
            Start = 3,
            Up = 4,
            Down = 5,
            Left = 6,
            Right = 7,
            MAX = 8
        };
        typedef std::array<bool, 8> Buttons;
        typedef std::function<bool (uint8_t, Button)> UpdateCallback;

    private:
        uint8_t _strobe = 0;
        uint8_t _joy1PollPosition = 0;
        uint8_t _joy2PollPosition = 0;

        UpdateCallback _updateCallback;

    public:
        Controller() {}

        void setUpdateCallback(UpdateCallback callback);

        void writeStrobe(uint8_t byte);

        uint8_t readJoy1();
        uint8_t readJoy2();
    };
}
