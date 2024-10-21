#include <SDL2/SDL.h>
#include "graphics.h"

int graphicsInit(SDL_Window** window, SDL_Renderer** renderer)
{
	int retVal = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	*window = SDL_CreateWindow("Felix's GB Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, RESOLUTION_X * RESOLUTION_SCALE, 
		RESOLUTION_Y * RESOLUTION_SCALE, SDL_WINDOW_SHOWN);
	*renderer = SDL_CreateRenderer(*window, -1, SDL_WINDOW_SHOWN);
	return retVal;
}
