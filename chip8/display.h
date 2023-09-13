#pragma once

#include "SDL.h"

class Display {
	SDL_Window* window;
	SDL_Renderer* renderer;

	int pixel_size;
	int width, height;

	bool pixels[32][64];

public:
	Display();
	~Display();

	void clear_screen();
	
	bool set_pixel(int x, int y, bool bit);

	void draw();
};