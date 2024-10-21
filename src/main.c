#include <stdio.h>
#include <SDL2/SDL.h>
#include "emu.h"
#include "graphics.h"

int main(int argc, char** argv)
{
	SDL_Event sEvent;
	SDL_Window* sWindow = NULL;
	SDL_Renderer* sRenderer = NULL;

	// TODO: Add function for loading a ROM and determining if it valid
	if (argc != 2)
	{
		printf("Usage: gameboy_emulator <rom_file>");
		return -1;
	}

	graphicsInit(&sWindow, &sRenderer);
	
	// Initialize Emulator context
	setEmuContextPaused(false);
	setEmuContextRunning(true);
	setEmuContextTicks(0);

	while(getEmuContext()->running)
	{
		if(getEmuContext()->paused)
		{
		}
	}

	return 0;
}
