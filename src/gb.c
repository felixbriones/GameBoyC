#include <stdio.h>
#include "gb.h"

#define GB_NUM_OF_OPCODES 512

//gb.c: opcodes/registers related to the GameBoy. emu.c: logic related specifically to emulation such as pausing/resuming execution

void invalid(gameBoy_t* gb)
{
	printf("Invalid opcode: %#04x\r\n");
}

void opNOP_0x00(gameBoy_t* gb)
{
	printf("opcode: \r\n");
}

/* 
 * LD BC,d16
 * 3  12
 * - - - -
 * Target: reg BC / Source 16-bit immediate value (next 2 bytes in memory)
 * 3 bytes long: 1 byte for opcode, 2 bytes for immediate value
 * Requires 12 clock cycles to complete
*/
void opLD_0x01(gameBoy_t* gb)
{
	printf("LD Executed\r\n");
}

void opLD_0x02(gameBoy_t* gb)
{
	printf("SUB Executed\r\n");
}

/*
 * @brief Consolidated table containing data for each operation supported by the LR35902 processor (Intel 8080 + Zilog Z80)
 * @details Contains the following data: { function pointer, cycles required, size of op code in bytes }
 */
struct gbInstruction gbDispatchTable[GB_NUM_OF_OPCODES] =
{
//	{ function,     cycles,  size } 
	{ opNOP_0x00,   4,       1    },  // NOP
	{ opLD_0x01,    12,      3    },  // LD BC, d16
	{ opLD_0x02,    8,       1    },  // LD (BC), A
};

void gbHandleCycle(gameBoy_t* gb)
{
	printf("Handling a cycle on the gameBoy");
		
	// If the execution time for the current operation has elapsed, move on to the next
	if(gb->cyclesCurrent == gb->cyclesTarget)
	{
		// Execute operation
		gbDispatchTable[gb->opCode].operation(gb);
		// Different opcodes have different lengths. Increment PC by length of most recent operation
		gb->pc+= gbDispatchTable[gb->opCode].opCodeSize * 8;
		// Set cyclesTarget so we know when to move on
		gb->cyclesTarget = gb->cyclesCurrent + gbDispatchTable[gb->opCode].clockCycles;
	}

	gb->cyclesCurrent++;
}
