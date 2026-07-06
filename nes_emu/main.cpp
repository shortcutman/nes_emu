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

#include "apu.hpp"
#include "cartridge.hpp"
#include "cpu.hpp"
#include "input_controller.hpp"
#include "memory.hpp"
#include "ppu.hpp"

namespace {
    const uint32_t ScreenWidth = 256;
    const uint32_t ScreenHeight = 240;

    std::array<int, nes_emu::Controller::Button::MAX> keyboardMappings {
        SDL_SCANCODE_P,
        SDL_SCANCODE_O,
        SDL_SCANCODE_Y,
        SDL_SCANCODE_U,

        SDL_SCANCODE_W,
        SDL_SCANCODE_S,
        SDL_SCANCODE_A,
        SDL_SCANCODE_D
    };
}

int main(int argc, const char * argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
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

    SDL_AudioSpec audio_spec{
        .format = SDL_AUDIO_S16,
        .channels = 1,
        .freq = 44100
    };
    SDL_AudioStream* audio_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audio_spec, NULL, NULL);
    if (!audio_stream) {
        std::println("Failed to create audio stream: {}", SDL_GetError());
        return 1;
    }
    SDL_ResumeAudioStreamDevice(audio_stream);
    
    std::ifstream cartFile(argv[1], std::fstream::in | std::fstream::binary);
    
    if (cartFile.fail()) {
        std::cout << std::strerror(errno);
    }
    
    auto cart = nes_emu::Cartridge::cartridgeFromStream(cartFile);
    
    auto cpu = std::make_shared<nes_emu::CPU>();
    auto memory = std::make_shared<nes_emu::Memory>();
    auto ppu = std::make_shared<nes_emu::PPU>();
    auto apu = std::make_shared<nes_emu::APU>();
    auto controller = std::make_shared<nes_emu::Controller>();
    controller->setUpdateCallback(
        [] (uint8_t controller, nes_emu::Controller::Button button) -> bool {
            if (controller == 1) {
                return false;
            }

            const bool* keyStates = SDL_GetKeyboardState(NULL);
            return keyStates[keyboardMappings[button]];
        });
    
    bool run = true;
    
    cpu->setMemory(memory);
    memory->setAPU(apu);
    memory->setCartridge(cart);
    memory->setController(controller);
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

        auto frame = ppu->getFrame();
        SDL_UpdateTexture(texture, NULL, &frame[0], 4 * ScreenWidth);

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        auto aud_que = SDL_GetAudioStreamQueued(audio_stream);
        std::println("frame, aud_que sampels: {}", aud_que/sizeof(int16_t));

        // while (SDL_GetAudioStreamQueued(audio_stream) > (735 * sizeof(int16_t))) {
        //     continue;
        // }

        if (SDL_GetAudioStreamQueued(audio_stream) < (735 * sizeof(int16_t))) {
        auto res = SDL_PutAudioStreamData(audio_stream, apu->_samples.data(), apu->sample_counter * sizeof(int16_t));
        if (!res) {
            std::println("Failed to put audio data: {} {}", SDL_GetError(), SDL_AUDIO_FRAMESIZE(audio_spec));
        }
        std::println("{} sample bytes added for: {}", apu->sample_counter, SDL_GetAudioStreamQueued(audio_stream)/sizeof(int16_t));
        apu->clear_samples();
        }

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

    SDL_PauseAudioStreamDevice(audio_stream);
    
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    SDL_Quit();
    
    return 0;
}
