#include <stdio.h>
#include "gb.h"

#define GB_NUM_OF_OPCODES 512

//gb.c: opcodes/registers related to the GameBoy. emu.c: logic related specifically to emulation such as pausing/resuming execution

// Define generalized opcode function data type which will be used to take one of many opcode functions at a time
// Each function in function table needs to have the same function signature, but different opcodes interact with
// different registers. Best way to handle this is to pass a pointer to the entire CPU
typedef void gbOpCode (gameBoy_t* gb);

void NOP (gameBoy_t* gb)
{
	printf("NOP Executed\r\n");
}

void ADD (gameBoy_t* gb)
{
	printf("ADD Executed\r\n");
}

void SUB (gameBoy_t* gb)
{
	printf("SUB Executed\r\n");
}

gbOpCode* gbDispatchTable[GB_NUM_OF_OPCODES] =
{
	NOP,
	ADD,
	SUB
};

void gbHandleCycle(gameBoy_t* gb)
{
	printf("Handling a cycle on the gameBoy");
}
