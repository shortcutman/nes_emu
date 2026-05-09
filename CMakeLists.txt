cmake_minimum_required(VERSION 4.0)

project(nes_emu)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_COMPILE_WARNING_AS_ERROR True)

add_compile_options(-Wall)

add_library(libnes
    instructions/cartridge.cpp
    instructions/cpu.cpp
    instructions/instruction_utils.cpp
    instructions/instructions_opcodes.cpp
    instructions/memory.cpp
    instructions/ppu_render.cpp
    instructions/ppu.cpp
    instructions/registers.cpp
)
target_include_directories(libnes PUBLIC instructions/)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(nes_test
    nes_emu-tests/cart_test.cpp
    nes_emu-tests/cpu_test.cpp
    nes_emu-tests/integration_test.cpp
    nes_emu-tests/interrupt_test.cpp
    nes_emu-tests/main.cpp
    nes_emu-tests/ppu_render_test.cpp
    nes_emu-tests/ppu_test.cpp
    nes_emu-tests/util_test.cpp
)
target_link_libraries(
    nes_test
    libnes
    GTest::gtest_main
    GTest::gmock
)
include(GoogleTest)
gtest_discover_tests(nes_test)

find_package(SDL3 REQUIRED)
add_executable(
    dannes
    nes_emu/main.cpp
)

target_link_libraries(dannes PRIVATE
    SDL3::SDL3
    libnes
    "-framework Foundation"
    "-framework Metal"
    "-framework QuartzCore"
)
