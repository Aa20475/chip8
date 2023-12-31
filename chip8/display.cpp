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
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
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
    pixels[y][x] ^= bit;
    return !pixels[y][x];
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

bool Display::handle_events(bool (&key_info)[16], bool &waiting_for_key, int& pressed_key) {
    SDL_Event event;
    bool quit = false;
    while (SDL_PollEvent(&event)) {
        
       //User requests quit
        if (event.type == SDL_QUIT)
            quit = true;
        
        if (event.type == SDL_KEYDOWN) {
            if (key_map.contains(event.key.keysym.scancode)) {
                std::cout << "Key " << key_map[event.key.keysym.scancode] << " down" << std::endl;
                if (waiting_for_key) {
                    pressed_key = event.key.keysym.scancode;
                    waiting_for_key = false;
                }
                key_info[key_map[event.key.keysym.scancode]] = true;
            }
        }
        if (event.type == SDL_KEYUP) {
            if (key_map.contains(event.key.keysym.scancode)) {
                std::cout << "Key " << key_map[event.key.keysym.scancode] << " up" << std::endl;
                key_info[key_map[event.key.keysym.scancode]] = false;
            }
        }
    }

    return quit;
}