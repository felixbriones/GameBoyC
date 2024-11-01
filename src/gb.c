#include <stdio.h>
#include "gb.h"

#define GB_NUM_OF_OPCODES 512

//gb.c: opcodes/registers related to the GameBoy. emu.c: logic related specifically to emulation such as pausing/resuming execution

// Define generalized opcode function data type which will be used to take one of many opcode functions at a time
// Each function in function table needs to have the same function signature, but different opcodes interact with
// different registers. Best way to handle this is to pass a pointer to the entire CPU
typedef void gbOpCode (gameBoy_t* gb);


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

gbOpCode* gbDispatchTable[GB_NUM_OF_OPCODES] =
{
	opNOP_0x00, // NOP
	opLD_0x01,  // LD BC, d16
	opLD_0x02,  // LD (BC), A
};

// Keeps track of required cycles required by each opcode
uint8_t gbOpcodeExecutionTime [512] =
{
	4,  // NOP (0x00)
	12, // LD  (0x01)	
	8,  // LD  (0x02)
};

void gbHandleCycle(gameBoy_t* gb)
{
	printf("Handling a cycle on the gameBoy");
		
	// If the execution time for the current operation has elapsed, move on to the next
	if(gb->cyclesCurrent == gb->cyclesTarget)
	{
		// Execute operation
		gbDispatchTable[gb->opCode](gb);
		// Different opcodes have different lengths. Increment PC by length of most recent operation
		gb->pc+= gbOpcodeExecutionTime[gb->opCode];
		// Set cyclesTarget so we know when to move on
		gb->cyclesTarget = gb->cyclesCurrent + gbOpcodeExecutionTime[gb->opCode];
	}

	gb->cyclesCurrent++;
}
