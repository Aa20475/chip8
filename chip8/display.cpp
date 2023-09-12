#include "display.h"
#include "SDL.h"
#include<stdexcept>
#include<iostream>

Display::~Display() {
	SDL_DestroyWindow(window);
}

Display::Display() : pixel_size(5), width(64), height(32) {
    std::cout << "Initializing SDL..." << std::endl;
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        throw std::runtime_error("SDL Init Error : "+ std::string(SDL_GetError()));
    }

    window = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, pixel_size * width, pixel_size * height, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        throw std::runtime_error("Window could not be created! SDL_Error:" + std::string(SDL_GetError()));
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
}