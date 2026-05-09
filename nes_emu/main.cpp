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

#include <SDL3/SDL.h>

#include "cartridge.hpp"
#include "cpu.hpp"
#include "memory.hpp"
#include "ppu.hpp"

namespace {
    const uint32_t ScreenWidth = 256;
    const uint32_t ScreenHeight = 240;
}

int main(int argc, const char * argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::println("Could not initialise SDL: {}", SDL_GetError());
        return -1;
    }
    
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_CreateWindowAndRenderer("dannes", ScreenWidth * 2, ScreenHeight * 2, 0, &window, &renderer);

    SDL_SetRenderLogicalPresentation(renderer, ScreenWidth, ScreenHeight, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        ScreenWidth, ScreenHeight);
    
    std::ifstream cartFile(argv[1], std::fstream::in | std::fstream::binary);
    
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
    
    auto last = std::chrono::high_resolution_clock::now();
    
    memory->setGameLoopCallback([&] () {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_EVENT_QUIT) {
                run = false;
            }
        }

        auto frame = ppu->renderFrame();
        SDL_UpdateTexture(texture, NULL, &frame[0], 4 * ScreenWidth);

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
        
        auto now = std::chrono::high_resolution_clock::now();
        auto diff = now - last;
        std::chrono::milliseconds frameTime(1000 / 60);
        auto sleepTime = std::chrono::duration_cast<std::chrono::milliseconds>(frameTime - diff);
        std::this_thread::sleep_for(sleepTime);
        
        last = std::chrono::high_resolution_clock::now();
    });

    uint64_t instructionNumber = 0;
    while (run) {
        instructionNumber++;
        cpu->executeOne();
    }

    std::println("Instruction count: {}", instructionNumber);
    
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    SDL_Quit();
    
    return 0;
}
