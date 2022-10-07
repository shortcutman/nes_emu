//
//  main.m
//  nes_emu
//
//  Created by Daniel Burke on 13/1/2022.
//

#include <iostream>
#include <fstream>

#include <chrono>
#include <thread>


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wquoted-include-in-framework-header"

//overall project setup based on: http://matthewstyles.com/set-up-an-sdl2-project-in-xcode/
#import <SDL2/SDL.h>

#pragma clang diagnostic pop

#include "cartridge.hpp"
#include "cpu.hpp"
#include "memory.hpp"
#include "ppu.hpp"

namespace {
    const uint32_t ScreenWidth = 256;
    const uint32_t ScreenHeight = 240;
}

int main(int argc, const char * argv[]) {
    
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cout << "failed to init sdl" << std::endl;
        return -1;
    }
    
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_CreateWindowAndRenderer(ScreenWidth * 2, ScreenHeight * 2, SDL_WINDOW_SHOWN, &window, &renderer);
    SDL_SetWindowTitle(window, "danes");

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, ScreenWidth, ScreenHeight);

    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        ScreenWidth, ScreenHeight);
    
    
    std::ifstream cartFile("", std::fstream::in | std::fstream::binary);
    
    if (cartFile.fail()) {
        std::cout << std::strerror(errno);
    }
    
    auto cart = nes_emu::Cartridge::cartridgeFromStream(cartFile);
    
    auto cpu = std::make_shared<nes_emu::CPU>();
    auto memory = std::make_shared<nes_emu::Memory>();
    auto ppu = std::make_shared<nes_emu::PPU>();
    
    bool run = true;
    
    cpu->setMemory(memory);
    memory->setCartridge(cart);
    memory->setPPU(ppu);
    ppu->setCartridge(cart);
    
    auto result = memory->read_uint16(0xFFFC);
    cpu->setPC(result);
  
//    SDL_Event event;
//    while (run) {
//        while (SDL_PollEvent(&event) != 0) {
//            if (event.type == SDL_QUIT) {
//                run = false;
//            }
//        }
//
//        auto frame = ppu->renderPatternTableToFrame();
//        SDL_UpdateTexture(texture, NULL, &frame[0], 3 * ScreenWidth);
//
//        SDL_RenderClear(renderer);
//        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
//        SDL_RenderPresent(renderer);
//    }
    
    memory->setGameLoopCallback([&] () {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                run = false;
            }
        }

        auto frame = ppu->renderFrame();
        SDL_UpdateTexture(texture, NULL, &frame[0], 4 * ScreenWidth);

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
        std::this_thread::sleep_for(std::chrono::milliseconds(33));

    });

    uint64_t instructionNumber = 0;
    while (run) {
        instructionNumber++;
        cpu->executeOne();
//        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    SDL_Quit();
    
    return 0;
}
