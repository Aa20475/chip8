#pragma once

#include "SDL.h"
#include<unordered_map>

class Display {
	SDL_Window* window;
	SDL_Renderer* renderer;

	int pixel_size;
	int width, height;

	bool pixels[32][64];

	std::unordered_map<int, int> key_map = {
		{SDL_SCANCODE_1, 0},
		{SDL_SCANCODE_2, 1},
		{SDL_SCANCODE_3, 2},
		{SDL_SCANCODE_4, 3},
		{SDL_SCANCODE_Q, 4},
		{SDL_SCANCODE_W, 5},
		{SDL_SCANCODE_E, 6},
		{SDL_SCANCODE_R, 7},
		{SDL_SCANCODE_A, 8},
		{SDL_SCANCODE_S, 9},
		{SDL_SCANCODE_D, 10},
		{SDL_SCANCODE_F, 11},
		{SDL_SCANCODE_Z, 12},
		{SDL_SCANCODE_X, 13},
		{SDL_SCANCODE_C, 14},
		{SDL_SCANCODE_V, 15},
	};

public:
	Display();
	~Display();

	void clear_screen();
	
	bool set_pixel(int x, int y, bool bit);

	void draw();

	bool handle_events(bool (&key_info)[16], bool& waiting_for_key, int& pressed_key);
};