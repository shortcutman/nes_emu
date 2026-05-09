//
//  controller_test.cpp
//
//  Created by Daniel Burke on 9/5/2026.
//

#include <gtest/gtest.h>

#include "input_controller.hpp"

using namespace nes_emu;

TEST(Controller, check_strobe_always_returns_a) {
    Controller c;
    uint8_t lastjoy = 8;
    Controller::Button lastbutton = Controller::Button::Right;
    c.setUpdateCallback([&] (uint8_t joy, Controller::Button button) {
        lastjoy = joy;
        lastbutton = button;
        return button == Controller::Button::A ? true : false;
    });

    c.writeStrobe(0x1);

    c.readJoy1();
    EXPECT_EQ(lastjoy, 0);
    EXPECT_EQ(lastbutton, Controller::Button::A);

    c.readJoy1();
    EXPECT_EQ(lastjoy, 0);
    EXPECT_EQ(lastbutton, Controller::Button::A);

    c.readJoy2();
    EXPECT_EQ(lastjoy, 1);
    EXPECT_EQ(lastbutton, Controller::Button::A);

    c.readJoy2();
    EXPECT_EQ(lastjoy, 1);
    EXPECT_EQ(lastbutton, Controller::Button::A);
}
