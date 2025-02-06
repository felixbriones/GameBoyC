#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "gb.h"

/*
 * cart.c: Contains logic related to loading and managing Game ROM as well as 
 * parsing ROM header for info like title, cart type, and ROM size.
 * Also contains logic relating to memory banking(MBC), SRAM management, and 
 * battery-backed save files
 */

void bootSequence(void)
{

}

/*
 * @brief Loads game ROM into the GameBoy's memory
 * @return null
 * @note This must be called after the Boot ROM BIOS sequence, as 
 * that sequence uses the first 256 bytes of memory during execution
 */
void cartLoadRom(gameBoy_t* gb, const char* gameRom)
{
	FILE* file = fopen(gameRom, "rb");
	uint32_t romSize = 0;

	if (file == NULL)
	{
		printf("Loading ROM NULL error\r\n");
		exit(1);
	}

	fseek(file, 0, SEEK_END);
	romSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (romSize > GB_MEMORY_SIZE)
	{
		printf("Game ROM too large error\r\n");
		fclose(file);
		exit(1);
	}

	// Load ROM into memory. 
	// TODO: Integrate logic for memory bank switching
	fread(gb->memory, sizeof(uint8_t), romSize, file);
	fclose(file);

}
