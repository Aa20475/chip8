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

    // Initialize Window
    window = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, pixel_size * width, pixel_size * height, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        throw std::runtime_error("Window could not be created! SDL_Error:" + std::string(SDL_GetError()));
    }

    // Initialize renderer
    renderer = SDL_CreateRenderer(window, -1, 0);
    // Initialise Pixel array
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++) {
            pixels[i][j] = false;
        }
    }
}

void Display::clear_screen() {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            pixels[i][j] = false;
        }
    }
}

bool Display::set_pixel(int x, int y, bool bit) {
    bool curr_bit = pixels[y][x];
    if (bit) {
        pixels[y][x] = !curr_bit;
        return curr_bit;
    }
    return false;
}

void Display::draw() {
    SDL_RenderClear(renderer);
    SDL_Rect rect;

    rect.w = pixel_size;
    rect.h = pixel_size;

    SDL_SetRenderDrawColor(renderer, 255,255,255,255);

    for (int x = 0; x < 64; x++) {
        for (int y = 0; y < 32; y++) {
            rect.x = x * pixel_size;
            rect.y = y * pixel_size;
            if(pixels[y][x])SDL_RenderFillRect(renderer, &rect);
        }
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderPresent(renderer);
}

bool Display::handle_events() {
    SDL_Event event;
    bool quit = false;
    while (SDL_PollEvent(&event)) {
        /* handle your event here */

       //User requests quit
        if (event.type == SDL_QUIT)
            quit = true;
    }

    return quit;
}