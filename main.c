#include <SDL2/SDL.h>
#include <stdbool.h>
#include <assert.h>
#include "opcodes.h"
#include "CPU.h"
#include "Bus.h"
#include "tests.h"

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("NES", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    bool running = true;
    SDL_Event event;


    assert(LDA_Test());


    while (running){
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_Quit();
    
    return 0;
}