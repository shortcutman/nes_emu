//
//  main.m
//  nes_emu
//
//  Created by Daniel Burke on 13/1/2022.
//

#include <iostream>
#include <fstream>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wquoted-include-in-framework-header"

//overall project setup based on: http://matthewstyles.com/set-up-an-sdl2-project-in-xcode/
#import <SDL2/SDL.h>

#pragma clang diagnostic pop

#include "cartridge.hpp"
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
        SDL_PIXELFORMAT_RGB24,
        SDL_TEXTUREACCESS_STREAMING,
        ScreenWidth, ScreenHeight);
    
    SDL_Event event;
    
    std::ifstream cartFile("", std::fstream::in | std::fstream::binary);
    
    if (cartFile.fail()) {
        std::cout << std::strerror(errno);
    }
    
    auto cart = nes_emu::Cartridge::cartridgeFromStream(cartFile);
    
    nes_emu::PPU ppu;
    ppu.setCartridge(cart);
    
    bool run = true;
    while (run) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                run = false;
            }
        }
        
        auto frame = ppu.renderFrame();
        SDL_UpdateTexture(texture, NULL, &frame[0], 3 * ScreenWidth);
        
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }
    
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    SDL_Quit();
    
    return 0;
}
