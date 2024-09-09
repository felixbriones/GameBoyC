/* emu.c: Contains logic relating to the runtime state of the emulator,
 * such as running, halted, etc. Useful when it comes to debugging 
 */

#include <stdbool.h>
#include <stdint.h>
#include "emu.h"

/* Emu components:
 * Cart: Load, read, write from cartridge
 * CPU: Emulate different instructions and registers
 * Address Bus: Reading and writing to address which are generally memory mapped
 * PPU: Pixel Processing Unit. Responsible for writing pixels to the screen
 * Timer: Used in several parts of the GB
 */

static emuContext_t context;

emuContext_t* getEmuContext(void)
{
	return &context;
}

void setEmuContextPaused(bool newVal)
{
	context.paused = newVal;
}

void setEmuContextRunning(bool newVal)
{
	context.running = newVal;
}

void setEmuContextTicks(uint64_t newVal)
{
	context.ticks = newVal;
}
