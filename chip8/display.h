#pragma once

#include "SDL.h"

class Display {
	SDL_Window* window;
	SDL_Renderer* renderer;

	int pixel_size;
	int width, height;

public:
	Display();
	~Display();
};