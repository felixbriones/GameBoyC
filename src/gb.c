#include <stdio.h>
#include "gb.h"

#define GB_NUM_OF_OPCODES 512

//gb.c: opcodes/registers related to the GameBoy. emu.c: logic related specifically to emulation such as pausing/resuming execution

/*
 * @brief Handles invalid or unknown opcodes and prints it
 * @param Pointer to gb struct containing registers
 * @return void
 * @note Shouldn't exectute under normal circumstances
 */
void invalid(gameBoy_t* gb)
{
	printf("Invalid opcode: %#04x\r\n");
}

/*
 * @brief Op code function for No Operation instruction (0x00): NOP 
 * @details Performs no work but consumes 4 clock cycles
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 4 cycles to execute
 */
void opNOP_0x00(gameBoy_t* gb)
{
	printf("opcode: \r\n");
}

/*
 * @brief Op code function for Load instruction (0x01): LD BC, d16
 * @details Loads 16-bit immediate value into register BC
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 3 bytes long and requires 12 cycles to execute
 */
void opLD_0x01(gameBoy_t* gb)
{
	printf("LD 0x01 Executed\r\n"); 
	uint16_t value = gb->memory[gb->pc + 1] | (gb->memory[gb->pc + 2] << 8);
	gb->generalReg.bc = value;
}

/*
 * @brief Op code function for Load instruction (0x02): LD (BC), A
 * @details Loads the value from register A to memory address specified by BC
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 8 cycles to execute
 */
void opLD_0x02(gameBoy_t* gb)
{
	printf("LD 0x02 Executed\r\n"); 
	uint8_t value = gb->generalReg.a;
	gb->memory[gb->generalReg.bc] = value;
}

/*
 * @brief Op code function for Load instruction (0x06): LD B, d8
 * @details Loads 8-bit immediate value into register B
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 2 bytes long and requires 8 cycles to execute
 */
void opLD_0x06(gameBoy_t* gb)
{
	printf("LD 0x06 Executed\r\n"); 
	uint8_t value = gb->memory[gb->pc + 1];
	gb->generalReg.b = value;
}

/*
 * @brief Op code function for Load instruction (0x08): LD (a16), SP
 * @details Loads the stack pointer into a 16-bit memory address
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 3 bytes long and requires 20 cycles to execute
 */
void opLD_0x08(gameBoy_t* gb)
{
	printf("LD 0x08 Executed\r\n"); 
	uint16_t value = gb->sp;
	uint16_t memAddr = gb->memory[gb->pc + 1] | (gb->memory[gb->pc + 2] << 8);
	// memory is uint8_t array, but sp is uint16_t. Store in 8-bit chunks (little-endian)
	gb->memory[memAddr] = value & 0xFF;
	gb->memory[memAddr + 1] = value >> 8;
}

/*
 * @brief Op code function for Load instruction (0x0A): LD A, (BC) 
 * @details Loads the value from a memory address specified by BC to register A
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 1 byte long and requires 8 cycles to execute
 */
void opLD_0x0A(gameBoy_t* gb)
{
	printf("LD 0x08 Executed\r\n"); 
	uint8_t value = gb->memory[gb->generalReg.bc];
	gb->generalReg.a = value;
}

/*
 * @brief Op code function for Load instruction (0x0A): LD C, d8 
 * @details Loads the 8-bit value to register C
 * @param Pointer to gb struct containing registers
 * @return void
 * @note This instruction is 2 byte long and requires 8 cycles to execute
 */
void opLD_0x0E(gameBoy_t* gb)
{
	printf("LD 0x0E Executed\r\n"); 
	uint8_t value = gb->memory[gb->pc + 1];
	gb->generalReg.c = value;
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
	{ opLD_0x06,    8,       2    },  // LD B, d8
	{ opLD_0x08,    20,      3    },  // LD (a16), SP
	{ opLD_0x0A,    8,       1    },  // LD A, (BC) 
	{ opLD_0x0E,    8,       2    },  // LD C, d8 
};

/*
 * @brief Function for handling the emulator's dispatch process (fetching, decoding, executing)
 * @details Uses a function/dispatch table to minimize time complexity by mapping each function pointer's index to their 
	corresponding op code
 * @param Pointer to gb struct containing registers
 * @return void
 */
void gbHandleCycle(gameBoy_t* gb)
{
	printf("Handling a cycle on the gameBoy");
		
	// If the execution time for the current operation has elapsed, move on to the next
	if(gb->cyclesCurrent == gb->cyclesTarget)
	{
		// Execute operation
		gbDispatchTable[gb->opCode].operation(gb);
		// Different opcodes have different lengths. Increment PC by length of most recent operation in bytes
		gb->pc+= gbDispatchTable[gb->opCode].opCodeSize;
		// Set cyclesTarget so we know when to move on
		gb->cyclesTarget = gb->cyclesCurrent + gbDispatchTable[gb->opCode].clockCycles;
	}

	gb->cyclesCurrent++;
}
