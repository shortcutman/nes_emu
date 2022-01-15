//
//  main.m
//  nes_emu
//
//  Created by Daniel Burke on 13/1/2022.
//

#include <iostream>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wquoted-include-in-framework-header"

//overall project setup based on: http://matthewstyles.com/set-up-an-sdl2-project-in-xcode/
#import <SDL2/SDL.h>

#pragma clang diagnostic pop

int main(int argc, const char * argv[]) {
    
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cout << "failed to init sdl" << std::endl;
        return -1;
    }
    
    
    
    std::cout << "hooray" << std::endl;
    SDL_Quit();
    
    return 0;
}
