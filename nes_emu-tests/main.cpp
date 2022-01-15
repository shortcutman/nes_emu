//
//  main.cpp
//  nes_emu-tests
//
//  Created by Daniel Burke on 14/1/2022.
//

//setup in xcode based on https://stackoverflow.com/questions/65775984/how-to-run-gtest-googletest-for-c-c-application-in-xcode-12-and-macos-big-su
#include <gtest/gtest.h>

int main(int argc, const char * argv[]) {
    testing::InitGoogleTest(&argc, (char**)argv);
    return RUN_ALL_TESTS();
}
